#include <chrono>
#include <thread>
#include <atomic>

namespace GearX {
    class FrameRateController {
    private:
        using Clock = std::chrono::high_resolution_clock;
        using TimePoint = std::chrono::time_point<Clock>;
        using Duration = std::chrono::duration<double>;

        // 目标帧率和帧间隔
        double m_targetFPS;
        Duration m_frameDuration;

        // 计时变量
        TimePoint m_lastFrameTime;
        Duration m_frameTime;
        Duration m_deltaTime;

        // 性能统计
        double m_avgFPS;
        std::vector<double> m_fpsHistory;
        size_t m_historySize;
        size_t m_frameCount;
        TimePoint m_startTime;

    public:
        // 构造函数：指定目标帧率和FPS历史记录大小
        explicit FrameRateController(double targetFPS = 60.0, size_t historySize = 60)
            : m_targetFPS(targetFPS),
            m_frameDuration(Duration(1.0 / targetFPS)),
            m_avgFPS(targetFPS),
            m_historySize(historySize),
            m_frameCount(0) {
            m_fpsHistory.reserve(historySize);
            reset();
        }

        // 重置控制器
        void reset() {
            m_lastFrameTime = Clock::now();
            m_frameTime = Duration(0);
            m_deltaTime = Duration(0);
            m_frameCount = 0;
            m_fpsHistory.clear();
            m_startTime = m_lastFrameTime;
        }

        // 设置新的目标帧率
        void setTargetFPS(double targetFPS) {
            m_targetFPS = targetFPS;
            m_frameDuration = Duration(1.0 / targetFPS);
        }

        // 获取目标帧率
        double getTargetFPS() const {
            return m_targetFPS;
        }

        // 获取当前帧的时间间隔（秒）
        double getDeltaTime() const {
            return m_deltaTime.count();
        }

        // 获取平均FPS
        double getAverageFPS() const {
            return m_avgFPS;
        }

        // 获取总运行时间（秒）
        double getTotalTime() const {
            return Duration(Clock::now() - m_startTime).count();
        }

        // 获取总帧数
        size_t getFrameCount() const {
            return m_frameCount;
        }

        // 等待下一帧，返回实际帧间隔时间（秒）
        double waitForNextFrame() {
            // 计算当前帧时间
            TimePoint currentTime = Clock::now();
            m_frameTime = currentTime - m_lastFrameTime;
            m_deltaTime = m_frameTime;

            // 计算实际FPS并更新历史记录
            double currentFPS = 1.0 / m_frameTime.count();
            updateFPSHistory(currentFPS);

            // 如果帧处理过快，等待剩余时间
            if (m_frameTime < m_frameDuration) {
                auto sleepTime = m_frameDuration - m_frameTime;
                // 使用高精度睡眠
                std::this_thread::sleep_for(sleepTime);

                // 重新计算实际帧时间
                currentTime = Clock::now();
                m_frameTime = currentTime - m_lastFrameTime;
                m_deltaTime = m_frameTime;
                currentFPS = 1.0 / m_frameTime.count();
                updateFPSHistory(currentFPS);
            }

            // 更新上一帧时间
            m_lastFrameTime = currentTime;
            ++m_frameCount;

            return m_deltaTime.count();
        }

    private:
        // 更新FPS历史记录并计算平均值
        void updateFPSHistory(double fps) {
            if (m_fpsHistory.size() >= m_historySize) {
                m_fpsHistory.erase(m_fpsHistory.begin());
            }
            m_fpsHistory.push_back(fps);

            // 计算平均FPS
            double sum = 0.0;
            for (double f : m_fpsHistory) {
                sum += f;
            }
            m_avgFPS = sum / m_fpsHistory.size();
        }
    };
}