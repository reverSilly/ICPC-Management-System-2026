#include<string>
#include<string_view>
#include<vector>
#include<unordered_map>
#include<optional>
#include<iostream>
#include<compare>
#include<ext/pb_ds/assoc_container.hpp>
#include<ext/pb_ds/tree_policy.hpp>

namespace reverSilly::ICPC
{
	struct Time
	{
		int t;
	};
	struct Problem;
	struct Team;
	struct Submission
	{
		Problem *problem;
		Team *owner;
		enum class Status
		{
			Accepted=0,
			WrongAnswer,
			RuntimeError,
			TimeLimitExceed
		}status;
		bool isAccepted()const;
		Time time;
	};
	struct Problem
	{
		std::string name;
		int id;
	};
	struct ProblemStatus//status for problem of every group
	{
		Problem *const problem;
		bool accepted;
		int tries;
		std::vector<Submission>submissions[4];//assert count(Submission status)=4
		struct FrozenInfo
		{
			int triesBefore;
			int triesAfter;
		};
		std::optional<FrozenInfo>frozen;//tries after frozen
		
		std::string str()const;//check: frozen and triesAfter neq 0->frozen, else regular.
		void freeze();//set frozen
		void unfreeze();//reset frozen
		bool isFrozen()const;
		void add(Submission s);//update accepted/tries/submissions
		bool isAccepted()const;//wrapper:reply no for frozen
		Time acceptedTime()const;//wrapper submits[Accepted].front().time
		Time penalty()const;//0 if not accepted/frozen, tries*20+AcTime
	};
	struct Team
	{
		std::string const name;
		std::vector<ProblemStatus>status;
		Time penalty()const;//sigma status.penalty()
		int accepted()const;//sigma status.accepted;
		void add(Submission s);//status.add
		friend std::strong_ordering operator<=>(const Team &a,const Team &b);//accepted|penalty|pass_time...|name
	};
	template<typename T>
	using RankedSet=__gnu_pbds::tree<T,__gnu_pbds::null_type,
									 std::less<int>,__gnu_pbds::rb_tree_tag,
									 __gnu_pbds::tree_order_statistics_node_update>;
	struct TeamRef
	{
		Team *t;
		friend bool operator<(TeamRef a,TeamRef b){return *a.t<*b.t;}
	};
	struct Contest
	{
		struct TeamInfo
		{
			using Set=RankedSet<TeamRef>;
			Set rank;
			std::unordered_map<std::string,std::pair<Team,Set::iterator>>teams;

			Team *teamByRank(int rank)const;//teams.rank();
			Team *teamByName(std::string_view)const;//Teams

			void addTeam(Team t);//rank,teams
			void start(int n);//team.status
			void refresh(Team *team,std::function<void(Team&)>);//erase iterator,do,insert iterator
		}teams;
		std::unordered_map<std::string,Problem> problems;
		std::optional<Time>duration;
		void addTeam();//Rankedteams, Teams
		void addProblem(Problem &&p);//problems
		void start(Time duration);//team.init, duration init.
		void flush(std::ostream&);//teams,status.accepted,team.penalty(),status.str()
		void freeze();//forevery team.status not accepted freeze
		void submit(Submission);//teams.refresh(add)
		void scroll(std::ostream&);//flush,team.refresh(unfreeze)...,flush
		int rankByTeam(Team *team);//teams.find
		Submission *query(Team *team,Problem *problem,Submission::Status)const;
		//team.status
		void end();//clear duration
	};
}