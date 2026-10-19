#include<string>
#include<string_view>
#include<vector>
#include<unordered_map>
#include<optional>
#include<iostream>
#include<compare>
#include<utility>
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
		Problem const *problem;
		Team const *owner;
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
		Problem const*const problem;
		bool accepted;
		int tries;
		std::vector<Submission>submissions[4];//assert count(Submission status)=4
		
		ProblemStatus(Problem const*p):problem{p}{}
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
		bool hasFrozen()const;
		friend std::strong_ordering operator<=>(const Team &a,const Team &b);//accepted|penalty|pass_time...|name
	};
	template<typename T>
	using RankedSet=__gnu_pbds::tree<T,__gnu_pbds::null_type,
									 std::less<int>,__gnu_pbds::rb_tree_tag,
									 __gnu_pbds::tree_order_statistics_node_update>;
	struct TeamRef
	{
		Team const*t;
		friend std::strong_ordering operator<=>(TeamRef a,TeamRef b){return *a.t<=>*b.t;}
	};
	struct Contest
	{
		struct TeamInfo
		{
			using Set=RankedSet<TeamRef>;
			Set rank;
			std::unordered_map<std::string,std::pair<Team,Set::iterator>>teams;

			Team const*teamByRank(int rank)const;//teams.rank();
			Team const*teamByName(std::string const&)const;//Teams
			int rankByTeam(Team const*)const;

			void addTeam(Team const&);//rank,teams
			void addProblem(Problem const*);//team.status
			void refresh(const std::string &team,std::function<void(Team&)>,Set*);//erase iterator,do,insert iterator
		}teams;
		std::unordered_map<std::string,Problem> problems;
		std::optional<Time>duration;
		bool frozen;

		void addTeam(Team const&);// wrapper
		void addProblem(Problem &&p);//problems, after all addTeam
		void start(Time duration);// duration init.
		void flush(std::ostream&)const;//teams,status.accepted,team.penalty(),status.str()
		void freeze();//forevery team.status not accepted freeze
		void submit(Submission);//teams.refresh(add)
		void scroll(std::ostream&);//flush,team.refresh(unfreeze)...,flush
		Submission *query(Team *team,Problem *problem,Submission::Status)const;
		//team.status
		void end();//clear duration

		void assertStarted(std::string const&)const;
		void assertNotStarted(std::string const&)const;
	};
}