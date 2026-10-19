#include"forward.h"
#include"Log.hpp"

#include<bits/stdc++.h>

using namespace reverSilly::ICPC;
using namespace std;

template<size_t n>
istream &operator>>(istream &is,char const(&arr)[n])
{
	string s;
	is>>s;
	assert(s==arr);
	return is;
}

Submission::Status fromStr(string const &s)
{
	using stat=Submission::Status;
	if(s=="Accepted")
		return stat::Accepted;
	if(s=="Wrong_Answer")
		return stat::WrongAnswer;
	if(s=="Runtime_Error")
		return stat::RuntimeError;
	if(s=="Time_Limit_Exceed")
		return stat::TimeLimitExceed;
	throw std::invalid_argument{"not supported :"+s};
}

std::string_view toStr(Submission::Status s)
{
	using stat=Submission::Status;
	switch(s)
	{
	case stat::Accepted:
		return "Accepted";
	case stat::WrongAnswer:
		return "Wrong_Answer";
	case stat::RuntimeError:
		return "Runtime_Error";
	case stat::TimeLimitExceed:
		return "Time_Limit_Exceed";
	default:
		throw std::invalid_argument{"not supported :"+to_string(int(s))};
	}
}

int lowbit(int a)
{
	return a&-a;
}

int main()
{
	string op;
	Contest c;
	while(cin>>op)
	try
	{
		string problem,team,status;
		int time;
		if(op=="ADDTEAM")//why not ADD_TEAM
		{
			cin>>team;
			c.addTeam(Team{.name=team});
			info("Add successfully.");
		}
		if(op=="START")
		{
			int dur,cnt;
			cin>>"DURATION">>dur>>"PROBLEM">>cnt;
			c.assertNotStarted("Start");
			for(int i=0;i<cnt;++i)
				c.addProblem(Problem{.name=string(1,'A'+i),.id=i});
			c.start({dur});
			info("Competition starts.");
		}
		if(op=="SUBMIT")
		{
			cin>>problem>>"BY">>team>>"WITH">>status>>"AT">>time;
			static int cnt=0;
			Submission s
			{
				.problem=&c.problems.find(problem)->second,
				.owner=c.teams.teamByName(team),
				.status=fromStr(status),
				.time={time},
				.id=cnt++
			};
			c.submit(s);
		}
		if(op=="FLUSH")
		{
			info("Flush scoreboard.");
			c.flush();
		}
		if(op=="FREEZE")
		{
			c.freeze();
			info("Freeze scoreboard.");
		}
		if(op=="SCROLL")
		{
			c.scroll(cout);
		}
		if(op=="QUERY_RANKING")
		{
			cin>>team;
			try
			{
				auto t=c.teams.teamByName(team);
				info("[Info]Complete query ranking.");
				if(c.frozen)
					warn("[Warning]Scoreboard is frozen. The ranking may be inaccurate until it were scrolled.");
				cout<<format("{} NOW AT RANKING {}\n",t->name,c.teams.rankByTeam(t)+1);
			}
			catch(const std::out_of_range& e)
			{
				error("Query ranking failed: cannot find the team.");
			}
		}
		if(op=="QUERY_SUBMISSION")
		{
			cin>>team>>"WHERE">>problem>>"AND">>status;
			problem=problem.substr(problem.find('=')+1);
			status=status.substr(status.find('=')+1);
			int pmask=0,smask=0;
			if(problem=="ALL")
				pmask=(1<<c.problems.size())-1;
			else
				pmask=1<<(problem[0]-'A');
			if(status=="ALL")
				smask=0b1111;
			else
				smask=1<<static_cast<int>(fromStr(status));
			try
			{
				auto t=c.teams.teamByName(team);
				info("Complete query submission.");//team exist=complete query?
				vector<Submission>candidate;
				for(unsigned int a=pmask;a;a-=lowbit(a))
				{
					int i=countr_zero(a);
					for(unsigned int b=smask;b;b-=lowbit(b))
					{
						int j=countr_zero(b);
						auto &subs=t->status.at(i).submissions[j];
						if(!subs.empty())
							candidate.push_back(subs.back());
					}
				}
				if(candidate.empty())
					cout<<"Cannot find any submission.";
				else
				{
					auto mx=
						max_element(candidate.rbegin(),candidate.rend(),
						[](Submission a,Submission b){return a.id<b.id;});
					cout<<t->name<<' '<<mx->problem->name<<' '<<toStr(mx->status)<<' '<<mx->time.t<<'\n';
				}
			}
			catch(std::out_of_range)
			{
				error("Query submission failed: cannot find the team.");
			}
		}
		if(op=="END")
		{
			c.end();
			info("Competition ends.");
		}
	}
	catch(std::exception const &e)
	{
		error(e.what());
	}
}