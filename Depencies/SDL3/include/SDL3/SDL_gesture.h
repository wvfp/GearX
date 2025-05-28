/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2022 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/* Touch gestures were removed from SDL3, so this is the SDL2 implementation copied in here, and tweaked a little. */

#ifndef INCL_SDL_GESTURE_H
#define INCL_SDL_GESTURE_H
#include <SDL3/SDL.h>
#define SDL_GESTURE_IMPLEMENTATION
#if !defined(SDL_MAJOR_VERSION)
#error Please include SDL.h before including this header.
#elif SDL_MAJOR_VERSION < 2
#error This header requires SDL2 or later.
#elif SDL_MAJOR_VERSION == 2
/* building against SDL2? Just use the built-in SDL2 implementation. */
#define Gesture_Init() (0)
#define Gesture_Quit()
#define Gesture_ID SDL_GestureID
#define Gesture_LoadDollarTemplates SDL_LoadDollarTemplates
#define Gesture_RecordGesture SDL_RecordGesture
#define Gesture_SaveAllDollarTemplates SDL_SaveAllDollarTemplates
#define Gesture_SaveDollarTemplate SDL_SaveDollarTemplate
#define GESTURE_DOLLARGESTURE SDL_DOLLARGESTURE
#define GESTURE_DOLLARRECORD SDL_DOLLARRECORD
#define GESTURE_MULTIGESTURE SDL_MULTIGESTURE
#define Gesture_MultiGestureEvent SDL_MultiGestureEvent
#define Gesture_DollarGestureEvent SDL_DollarGestureEvent
#else

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

typedef Sint64 Gesture_ID;

/* events... */

/* generally you shouldn't hardcode event type numbers--and doubly so in
   the reserved range!--but these match SDL2 and SDL3 promises to preserve
   these values to help sdl2-compat. */
#define GESTURE_DOLLARGESTURE 0x800
#define GESTURE_DOLLARRECORD 0x801
#define GESTURE_MULTIGESTURE 0x802

typedef struct Gesture_MultiGestureEvent
{
    Uint32 type;
    Uint32 timestamp;
    SDL_TouchID touchID;
    float dTheta;
    float dDist;
    float x;
    float y;
    Uint16 numFingers;
    Uint16 padding;
} Gesture_MultiGestureEvent;

typedef struct Gesture_DollarGestureEvent
{
    Uint32 type;
    Uint32 timestamp;
    SDL_TouchID touchID;
    Gesture_ID gestureId;
    Uint32 numFingers;
    float error;
    float x;
    float y;
} Gesture_DollarGestureEvent;


/* Function prototypes */

/**
 * Call this once, AFTER SDL_Init, to set up the Gesture API.
 *
 * \returns 0 on success, -1 on error. Call SDL_GetError() for specifics.
 */
extern int SDLCALL Gesture_Init(void);

/**
 * Call this once, BEFORE SDL_Quit, to clean up the Gesture API.
 */
extern void SDLCALL Gesture_Quit(void);

/**
 * Begin recording a gesture on a specified touch device or all touch devices.
 *
 * If the parameter `touchID` is -1 (i.e., all devices), this function will
 * always return 1, regardless of whether there actually are any devices.
 *
 * \param touchID the touch device id, or -1 for all touch devices
 * \returns 1 on success or 0 if the specified device could not be found.
 */
extern int SDLCALL Gesture_RecordGesture(SDL_TouchID touchID);

/**
 * Save all currently loaded Dollar Gesture templates.
 *
 * \param dst a SDL_IOStream to save to
 * \returns the number of saved templates on success or 0 on failure; call
 *          SDL_GetError() for more information.
 *
 * \since This function is available since SDL 2.0.0.
 *
 * \sa Gesture_LoadDollarTemplates
 * \sa Gesture_SaveDollarTemplate
 */
extern int SDLCALL Gesture_SaveAllDollarTemplates(SDL_IOStream *dst);

/**
 * Save a currently loaded Dollar Gesture template.
 *
 * \param gestureId a gesture id
 * \param dst a SDL_IOStream to save to
 * \returns 1 on success or 0 on failure; call SDL_GetError() for more
 *          information.
 *
 * \since This function is available since SDL 2.0.0.
 *
 * \sa SDL_LoadDollarTemplates
 * \sa SDL_SaveAllDollarTemplates
 */
extern int SDLCALL Gesture_SaveDollarTemplate(Gesture_ID gestureId, SDL_IOStream *dst);

/**
 * Load Dollar Gesture templates from a file.
 *
 * \param touchID a touch id
 * \param src a SDL_IOStream to load from
 * \returns the number of loaded templates on success or a negative error code
 *          (or 0) on failure; call SDL_GetError() for more information.
 *
 * \since This function is available since SDL 2.0.0.
 *
 * \sa SDL_SaveAllDollarTemplates
 * \sa SDL_SaveDollarTemplate
 */
extern int SDLCALL Gesture_LoadDollarTemplates(SDL_TouchID touchID, SDL_IOStream *src);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#if defined(SDL_GESTURE_IMPLEMENTATION)

#define GESTURE_MAX_DOLLAR_PATH_SIZE 1024
#define GESTURE_DOLLARNPOINTS 64
#define GESTURE_DOLLARSIZE 256
#define GESTURE_PHI        0.618033989

typedef struct
{
    float length;
    int numPoints;
    SDL_FPoint p[GESTURE_MAX_DOLLAR_PATH_SIZE];
} GestureDollarPath;

typedef struct
{
    SDL_FPoint path[GESTURE_DOLLARNPOINTS];
    unsigned long hash;
} GestureDollarTemplate;

typedef struct
{
    SDL_TouchID touchID;
    SDL_FPoint centroid;
    GestureDollarPath dollarPath;
    Uint16 numDownFingers;
    int numDollarTemplates;
    GestureDollarTemplate *dollarTemplate;
    bool recording;
} GestureTouch;

/* 静态全局变量 */
static GestureTouch* GestureTouches;
static int GestureNumTouches;
static bool GestureRecordAll;

/* 静态函数声明 */
static void GestureProcessEvent(const SDL_Event* event);
static bool SDLCALL GestureEventWatch(void* userdata, SDL_Event* event);
static GestureTouch* GestureAddTouch(SDL_TouchID touchID);
static int GestureDelTouch(SDL_TouchID touchID);
static GestureTouch* GestureGetTouch(SDL_TouchID touchID);
static unsigned long GestureHashDollar(SDL_FPoint* points);
static int GestureSaveTemplate(GestureDollarTemplate* templ, SDL_IOStream* dst);
static int GestureAddDollar_one(GestureTouch* inTouch, SDL_FPoint* path);
static int GestureAddDollar(GestureTouch* inTouch, SDL_FPoint* path);
static float GestureDollarDifference(SDL_FPoint* points, SDL_FPoint* templ, float ang);
static float GestureBestDollarDifference(SDL_FPoint* points, SDL_FPoint* templ);
static int GestureDollarNormalize(const GestureDollarPath* path, SDL_FPoint* points, bool is_recording);
static float GestureDollarRecognize(const GestureDollarPath* path, int* bestTempl, GestureTouch* touch);
static void GestureSendMulti(GestureTouch* touch, float dTheta, float dDist);
static void GestureSendDollar(GestureTouch* touch, Gesture_ID gestureId, float error);
static void GestureSendDollarRecord(GestureTouch* touch, Gesture_ID gestureId);

#endif  /* defined(SDL_GESTURE_IMPLEMENTATION) */
#endif  /* SDL version > 2 */
#endif /* INCL_SDL_GESTURE_H */

/* vi: set sts=4 ts=4 sw=4 expandtab: */
