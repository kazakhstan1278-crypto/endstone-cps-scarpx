#pragma once
#include <endstone/plugin/plugin.h>
#include <endstone/player.h>
#include <endstone/server.h>
#include <endstone/scheduler/scheduler.h>
#include <endstone/scheduler/task.h>
#include <endstone/event/player/player_interact_event.h>
#include <endstone/event/actor/actor_damage_event.h>
#include <endstone/event/player/player_quit_event.h>
#include <chrono>
#include <deque>
#include <memory>
#include <string>
#include <unordered_map>
class CpsGuard : public endstone::Plugin {
public:
    void onEnable() override;
    void onDisable() override;
    bool onCommand(endstone::CommandSender &, const endstone::Command &, const std::vector<std::string> &) override;
private:
    using Clock=std::chrono::steady_clock; using TimePoint=Clock::time_point;
    struct PlayerState { std::deque<TimePoint> attempts; TimePoint last_interact{}; bool last_interact_blocked=false; TimePoint last_log{}; };
    static constexpr auto Window=std::chrono::milliseconds(1000);
    static constexpr auto DedupWindow=std::chrono::milliseconds(75);
    static constexpr int SuspiciousThreshold=30;
    void onInteract(endstone::PlayerInteractEvent &); void onDamage(endstone::ActorDamageEvent &); void onQuit(endstone::PlayerQuitEvent &); void showHud();
    static std::string keyFor(const endstone::Player &); static void prune(PlayerState &,TimePoint);
    bool recordAttempt(endstone::Player &,PlayerState &,TimePoint); void logSuspicious(const std::string &,int); void loadConfig(); void saveConfig() const; void showLog(endstone::CommandSender &) const;
    std::unordered_map<std::string,PlayerState> states_; int limit_{15}; std::shared_ptr<endstone::Task> hud_task_;
};
