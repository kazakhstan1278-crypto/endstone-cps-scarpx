#include "plugin.h"
#include <algorithm>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <string_view>
namespace {
std::string trim(std::string s){auto a=s.find_first_not_of(" \t\r\n");if(a==std::string::npos)return{};auto b=s.find_last_not_of(" \t\r\n");return s.substr(a,b-a+1);}
int readInt(const std::filesystem::path& f,std::string_view k,int fallback){std::ifstream in(f);std::string l;while(std::getline(in,l)){l=trim(std::move(l));if(l.rfind(k,0)!=0)continue;auto e=l.find('=');if(e==std::string::npos)continue;try{return std::stoi(trim(l.substr(e+1)));}catch(...){return fallback;}}return fallback;}
std::string nowString(){auto n=std::chrono::system_clock::now();auto t=std::chrono::system_clock::to_time_t(n);std::tm tm{};localtime_r(&t,&tm);char b[32]{};std::strftime(b,sizeof(b),"%Y-%m-%d %H:%M:%S",&tm);return b;}
}
void CpsGuard::onEnable(){loadConfig();registerEvent(&CpsGuard::onInteract,*this,endstone::EventPriority::Highest,false);registerEvent(&CpsGuard::onDamage,*this,endstone::EventPriority::Highest,false);registerEvent(&CpsGuard::onQuit,*this);hud_task_=getServer().getScheduler().runTaskTimer(*this,[this]{showHud();},1,20);getLogger().info("CPSGuard enabled: limit={} CPS, suspicious threshold=30",limit_);}
void CpsGuard::onDisable(){if(hud_task_){hud_task_->cancel();hud_task_.reset();}saveConfig();states_.clear();}
bool CpsGuard::onCommand(endstone::CommandSender& s,const endstone::Command&,const std::vector<std::string>& a){
 if(a.empty()){s.sendMessage("§e/cps config <0-100> §7- set CPS limit");s.sendMessage("§e/cps log §7- show suspicious CPS log");s.sendMessage("§7Current CPS limit: §f{}",limit_);return true;}
 if(a[0]=="config"){if(a.size()==1){s.sendMessage("§eCPS limit: §f{}§7 (range 0-100)",limit_);return true;}if(a.size()!=2){s.sendErrorMessage("Usage: /cps config <0-100>");return true;}try{int v=std::stoi(a[1]);if(v<0||v>100){s.sendErrorMessage("CPS limit must be between 0 and 100.");return true;}limit_=v;saveConfig();s.sendMessage("§aCPS limit set to §f{}§a.",limit_);if(!limit_)s.sendMessage("§cPlayers cannot attack while the limit is 0.");}catch(...){s.sendErrorMessage("Usage: /cps config <0-100>");}return true;}
 if(a[0]=="log"){showLog(s);return true;} s.sendErrorMessage("Usage: /cps config <0-100> | /cps log");return true;}
std::string CpsGuard::keyFor(const endstone::Player&p){return p.getName();}
void CpsGuard::prune(PlayerState&s,TimePoint n){while(!s.attempts.empty()&&n-s.attempts.front()>=Window)s.attempts.pop_front();}
bool CpsGuard::recordAttempt(endstone::Player&p,PlayerState&s,TimePoint n){prune(s,n);s.attempts.push_back(n);int cps=(int)s.attempts.size();if(cps>=SuspiciousThreshold&&(s.last_log.time_since_epoch().count()==0||n-s.last_log>=Window)){s.last_log=n;logSuspicious(p.getName(),cps);}return limit_>0&&cps<=limit_;}
void CpsGuard::onInteract(endstone::PlayerInteractEvent&e){if(e.getAction()!=endstone::PlayerInteractEvent::Action::LeftClickAir)return;auto&p=e.getPlayer();auto n=Clock::now();auto&s=states_[keyFor(p)];bool ok=recordAttempt(p,s,n);s.last_interact=n;s.last_interact_blocked=!ok;if(!ok)e.cancel();}
void CpsGuard::onDamage(endstone::ActorDamageEvent&e){auto&src=e.getDamageSource();if(src.getType()!="entity_attack")return;auto*a=src.getDamagingActor();if(!a)a=src.getActor();if(!a)return;auto*p=dynamic_cast<endstone::Player*>(a);if(!p)return;auto n=Clock::now();auto&s=states_[keyFor(*p)];if(s.last_interact.time_since_epoch().count()!=0&&n-s.last_interact<=DedupWindow){if(s.last_interact_blocked)e.cancel();return;}if(!recordAttempt(*p,s,n))e.cancel();}
void CpsGuard::onQuit(endstone::PlayerQuitEvent&e){states_.erase(keyFor(e.getPlayer()));}
void CpsGuard::showHud(){for(auto*p:getServer().getOnlinePlayers())if(p)p->sendPopup("§7made by scarpx");}
void CpsGuard::loadConfig(){std::error_code ec;std::filesystem::create_directories(getDataFolder(),ec);auto f=getDataFolder()/"config.toml";if(!std::filesystem::exists(f)){saveConfig();return;}limit_=std::clamp(readInt(f,"limit",15),0,100);}
void CpsGuard::saveConfig()const{std::error_code ec;std::filesystem::create_directories(getDataFolder(),ec);std::ofstream o(getDataFolder()/"config.toml",std::ios::trunc);if(o)o<<"# CPSGuard - Endstone 0.11.12 / Bedrock 1.26.51\n[cps]\nlimit = "<<limit_<<"\n";}
void CpsGuard::logSuspicious(const std::string&n,int cps){std::error_code ec;std::filesystem::create_directories(getDataFolder(),ec);std::ofstream o(getDataFolder()/"cps.log",std::ios::app);if(o)o<<"["<<nowString()<<"] "<<n<<" - "<<cps<<" CPS\n";}
void CpsGuard::showLog(endstone::CommandSender&s)const{std::ifstream in(getDataFolder()/"cps.log");if(!in){s.sendMessage("§7CPS log is empty.");return;}std::deque<std::string> q;std::string l;while(std::getline(in,l)){if(!l.empty())q.push_back(l);if(q.size()>15)q.pop_front();}if(q.empty()){s.sendMessage("§7CPS log is empty.");return;}s.sendMessage("§e--- CPS suspicious log (last 15) ---");for(const auto&x:q)s.sendMessage("§7{}",x);}
ENDSTONE_PLUGIN("cpsguard","2.1.0",CpsGuard){description="CPS limiter and suspicious CPS logger";authors={"scarpx"};prefix="CPSGuard";permission("cpsguard.command").description("Use /cps").default_(endstone::PermissionDefault::Operator);command("cps").description("CPS limiter configuration and log").usages("/cps","/cps config <0-100>","/cps log").permissions("cpsguard.command");}
