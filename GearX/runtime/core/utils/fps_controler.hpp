#include <chrono>
#include <thread>
#include <atomic>

namespace GearX {
    class FrameRateController {
    private:
        using Clock = std::chrono::high_resolution_clock;
        using TimePoint = std::chrono::time_point<Clock>;
        using Duration = std::chrono::duration<double>;

        // Ŀ��֡�ʺ�֡���
        double m_targetFPS;
        Duration m_frameDuration;

        // ��ʱ����
        TimePoint m_lastFrameTime;
        Duration m_frameTime;
        Duration m_deltaTime;

        // ����ͳ��
        double m_avgFPS;
        std::vector<double> m_fpsHistory;
        size_t m_historySize;
        size_t m_frameCount;
        TimePoint m_startTime;

    public:
        // ���캯����ָ��Ŀ��֡�ʺ�FPS��ʷ��¼��С
        explicit FrameRateController(double targetFPS = 60.0, size_t historySize = 60)
            : m_targetFPS(targetFPS),
            m_frameDuration(Duration(1.0 / targetFPS)),
            m_avgFPS(targetFPS),
            m_historySize(historySize),
            m_frameCount(0) {
            m_fpsHistory.reserve(historySize);
            reset();
        }

        // ���ÿ�����
        void reset() {
            m_lastFrameTime = Clock::now();
            m_frameTime = Duration(0);
            m_deltaTime = Duration(0);
            m_frameCount = 0;
            m_fpsHistory.clear();
            m_startTime = m_lastFrameTime;
        }

        // �����µ�Ŀ��֡��
        void setTargetFPS(double targetFPS) {
            m_targetFPS = targetFPS;
            m_frameDuration = Duration(1.0 / targetFPS);
        }

        // ��ȡĿ��֡��
        double getTargetFPS() const {
            return m_targetFPS;
        }

        // ��ȡ��ǰ֡��ʱ�������룩
        double getDeltaTime() const {
            return m_deltaTime.count();
        }

        // ��ȡƽ��FPS
        double getAverageFPS() const {
            return m_avgFPS;
        }

        // ��ȡ������ʱ�䣨�룩
        double getTotalTime() const {
            return Duration(Clock::now() - m_startTime).count();
        }

        // ��ȡ��֡��
        size_t getFrameCount() const {
            return m_frameCount;
        }

        // �ȴ���һ֡������ʵ��֡���ʱ�䣨�룩
        double waitForNextFrame() {
            // ���㵱ǰ֡ʱ��
            TimePoint currentTime = Clock::now();
            m_frameTime = currentTime - m_lastFrameTime;
            m_deltaTime = m_frameTime;

            // ����ʵ��FPS��������ʷ��¼
            double currentFPS = 1.0 / m_frameTime.count();
            updateFPSHistory(currentFPS);

            // ���֡������죬�ȴ�ʣ��ʱ��
            if (m_frameTime < m_frameDuration) {
                auto sleepTime = m_frameDuration - m_frameTime;
                // ʹ�ø߾���˯��
                std::this_thread::sleep_for(sleepTime);

                // ���¼���ʵ��֡ʱ��
                currentTime = Clock::now();
                m_frameTime = currentTime - m_lastFrameTime;
                m_deltaTime = m_frameTime;
                currentFPS = 1.0 / m_frameTime.count();
                updateFPSHistory(currentFPS);
            }

            // ������һ֡ʱ��
            m_lastFrameTime = currentTime;
            ++m_frameCount;

            return m_deltaTime.count();
        }

    private:
        // ����FPS��ʷ��¼������ƽ��ֵ
        void updateFPSHistory(double fps) {
            if (m_fpsHistory.size() >= m_historySize) {
                m_fpsHistory.erase(m_fpsHistory.begin());
            }
            m_fpsHistory.push_back(fps);

            // ����ƽ��FPS
            double sum = 0.0;
            for (double f : m_fpsHistory) {
                sum += f;
            }
            m_avgFPS = sum / m_fpsHistory.size();
        }
    };
}