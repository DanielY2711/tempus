#include <chrono>

namespace track_activity{

    struct activityRating{
        long total;
        long active;
    };


    void startActivityTracking(std::chrono::seconds timeoutInterval);

    activityRating reportActivityThenReset();

}