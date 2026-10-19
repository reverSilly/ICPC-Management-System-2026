#include"forward.h"

namespace reverSilly::ICPC
{
	void Contest::assertStarted(std::string const &sv)const
	{
		if(!duration.has_value())
			throw std::logic_error{sv+" failed: competition not started"};
	}
	void Contest::assertNotStarted(std::string const &sv)const
	{
		if(duration.has_value())
			throw std::logic_error{sv+" failed: competition has started."};
	}

	void Contest::addTeam(Team const &t)
	{
		assertNotStarted("Add");
		teams.addTeam(t);
	}
	void Contest::addProblem(Problem &&p)
	{
		assertNotStarted("Add Problem");
		auto [it,_]=problems.insert({p.name,std::move(p)});
		if(!_)
			throw std::invalid_argument{"Problem already exist"};
		teams.addProblem(&it->second);
	}
	void Contest::start(Time duration)
	{
		assertNotStarted("Start");
		this->duration=duration;
		teams.rebuild();
	}
	void Contest::print(std::ostream &os)const
	{
		assertStarted("Print");
		int rank=0;
		for(auto &i:teams.rank)
		{
			auto &t=*teams.teamByName(std::string(i.name));
			os<<t.name<<' '<<rank+1<<' '<<t.accepted()<<' '<<t.penalty().t<<' ';
			for(auto &s:t.status)
				os<<s.str()<<' ';
			os<<'\n';
			++rank;
		}
	}
	void Contest::flush()
	{
		assertStarted("Flush");
		teams.rebuild();
	}
	void Contest::freeze()
	{
		assertStarted("Freeze");
		if(frozen)
			throw std::logic_error{"Freeze failed: scoreboard has been frozen."};
		for(auto &[k,v]:teams.teams)
		{
			auto &[team,it]=v;
			for(auto &p:team.status)
			{
				assert(!p.isFrozen());
				if(!p.isAccepted())
					p.freeze();
			}
		}
		frozen=true;
	}
	void Contest::submit(Submission s)
	{
		assertStarted("Submit");
		teams.refresh(s.owner->name,[s](Team &t){t.add(s);},nullptr);
	}
	void Contest::scroll(std::ostream &os)
	{
		assertStarted("Scroll");
		if(!frozen)
			throw std::logic_error{"Scroll failed: scoreboard has not been frozen."};
		frozen=false;
		os<<"[Info]Scroll scoreboard.\n";//this is not so elegant
		flush();
		print(os);
		TeamInfo::Set frozenTeam;
		for(auto&[k,v]:teams.teams)
		{
			if(v.first.hasFrozen())
				frozenTeam.insert({v.first});
		}
		while(!frozenTeam.empty())
		{
			auto t=teams.teamByName(std::string(frozenTeam.rbegin()->name));
			int rk1=teams.rankByTeam(t);
			teams.refresh(t->name,[](Team &t)
			{
				for(auto &p:t.status)
				{
					if(p.isFrozen())
					{
						p.unfreeze();
						return;
					}
				}
			},&frozenTeam);
			int rk2=teams.rankByTeam(t);
			if(!t->hasFrozen())
				frozenTeam.erase({*t});
			if(rk2!=rk1)
			{
				os<<t->name<<' ';
				if(rk1<rk2)
					os<<teams.teamByRank(rk2-1)->name<<' ';
				else if(rk2<rk1)
					os<<teams.teamByRank(rk2+1)->name<<' ';
				else assert(false);
				os<<t->accepted()<<' '<<t->penalty().t<<'\n';
			}
		}
		print(os);
	}
	Submission *Contest::query(Team *team,Problem *problem,Submission::Status s)const
	{
		assertStarted("Query");
		auto ss=static_cast<std::size_t>(s);
		auto &submits=team->status[problem->id].submissions;
		if(submits[ss].empty())
			throw std::out_of_range{"Cannot find any submission."};
		return &submits[ss].back();
	}
	void Contest::end()
	{
		assertStarted("End");
		duration.reset();
		assert(frozen==false);
	}
}