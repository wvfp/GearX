####################################################################################
#                                                                                  #
#  Copyright (c) 2014, 2015 - 2016 Axel Menzel <info@rttr.org>                     #
#                                                                                  #
#  This file is part of RTTR (Run Time Type Reflection)                            #
#  License: MIT License                                                            #
#                                                                                  #
#  Permission is hereby granted, free of charge, to any person obtaining           #
#  a copy of this software and associated documentation files (the "Software"),    #
#  to deal in the Software without restriction, including without limitation       #
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,        #
#  and/or sell copies of the Software, and to permit persons to whom the           #
#  Software is furnished to do so, subject to the following conditions:            #
#                                                                                  #
#  The above copyright notice and this permission notice shall be included in      #
#  all copies or substantial portions of the Software.                             #
#                                                                                  #
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      #
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        #
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     #
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          #
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   #
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   #
#  SOFTWARE.                                                                       #
#                                                                                  #
####################################################################################

####################################################################################
# Search and install 3rd party libraries
#
####################################################################################

MESSAGE(STATUS ${LIBRARY_OUTPUT_DIRECTORY})
MESSAGE(STATUS "Finding 3rd party libs...")
MESSAGE(STATUS "===========================")

# there is a the moment a problem with finding multiple versions of boost,
# i.e. the static AND the static runtime version; that is not possible atm.
# Because of that, the benchmarks cannot be build with the static runtime lib option enabled
set(Boost_USE_STATIC_LIBS       OFF)
set(Boost_USE_STATIC_RUNTIME    OFF)
set(BOOST_ALL_DYN_LINK          OFF) 

#find_package(Boost COMPONENTS chrono system)
find_package(Threads REQUIRED)
      
MESSAGE(STATUS "Finished finding 3rd party libs!")
