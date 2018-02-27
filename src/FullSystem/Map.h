#pragma once

#include "util/NumType.h"
#include "FullSystem/FrameHessian.h"
#include "FullSystem/PointHessian.h"
#include "FullSystem/ImmaturePoint.h"
#include "FullSystem/CalibHessian.h"
#include "FullSystem/Frame.h"
#include "IOWrapper/OutputWrapper/Output3DWrapper.h"

#include <set>
#include <thread>
#include <mutex>

using namespace std;
using namespace fdso;

namespace fdso {
class FullSystem;
// namespace IOWrap
// {
// class Output3DWrapper;
// }


/**
 * The global map contains all keyframes and map points, even if they are outdated.
 * The map can be saved to and loaded from disk, if you wanna reuse it.
 *
 * The loop closing thread will call the optimize function if there is a consistent loop closure.
 */

class Map {
public:
    Map(FullSystem* fullsystem);
    /**
     * add a keyframe into the global map
     * @param kf
     * 插入关键帧
     */
    void addKeyFrame(Frame* kf);//,SE3 current_2_slast=SE3());

    /**
     * optimize pose graph of all kfs
     * this will start the pose graph optimization thread (usually takes several seconds to return in my machine)
     * @param allKFs
     * 优化全部的关键帧
     */
    void optimizeALLKFs();

    /**
     * get number of frames stored in global map
     * @return
     * 帧的数量
     */
    inline int numFrames() const
    {
        return frames.size();
    }

    // is pose graph running?
    //是否在位姿图优化
    bool idle() {
        std::unique_lock<std::mutex> lock(mutexPoseGraph);
        return !poseGraphRunning;
    }

    //全部的关键帧
    std::set<Frame*, CmpFrameID> getAllKFs() { return frames; }


    int min_id;
    int max_id;

    std::vector<IOWrap::Output3DWrapper*> outputWrapper;
    std::vector<Frame*>  frameList;

private:
    // the pose graph optimization thread
    //位姿图优化
    void runPoseGraphOptimization();

    //地图互斥锁
    std::mutex mapMutex; // map mutex to protect its data


    //全部的关键帧包括ID
    std::set<Frame*, CmpFrameID> frames;  // all KFs by ID
    //关键帧被优化
    std::set<Frame*, CmpFrameID> framesOpti;  // KFs to be optimized
    //当前关键帧
    Frame* currentKF = nullptr;

    //是否在运行位姿图优化
    bool poseGraphRunning = false;  // is pose graph running?

    //位姿图互斥锁
    std::mutex mutexPoseGraph;

    CalibHessian* mpHcalib = nullptr;
};

}