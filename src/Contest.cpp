#include"forward.h"
#include<cassert>

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
		for(auto &[k,v]:teams.teams)
			v.second=teams.rank.insert(teams.rank.end(),v.first);
		teams.rank.sort();
		flush();
	}
	void Contest::print(std::ostream &os)const
	{
		assertStarted("Print");
		int rank=0;
		for(auto &i:teams.rank)
		{
			os<<i.t->name<<' '<<rank+1<<' '<<i.t->accepted()<<' '<<i.t->penalty().t<<' ';
			for(auto &s:i.t->status)
			{
				s.print(os);
				os<<' ';
			}
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
		teams.refresh(s.owner->name,[s](Team &t){t.add(s);});
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
		std::set<TeamRef>rank,frozenTeam;
		for(auto &i:teams.rank)
			rank.insert(i);
		for(auto&[k,v]:teams.teams)
		{
			if(v.first.hasFrozen())
				frozenTeam.insert({v.first});
			rank.insert({v.first});
		}
		while(!frozenTeam.empty())
		{
		retry:
			auto it=teams.teams.find(std::string(frozenTeam.rbegin()->name));
			auto t=&it->second.first;
			auto jt=rank.find({it->second.first});
			std::optional<TeamRef>pv,nx;
			if(jt!=rank.begin())
				pv=*prev(jt);
			if(next(jt)!=rank.end())
				nx=*next(jt);
			if(it->second.second)
			{
				teams.rank.erase(*it->second.second);
				teams.dirty.insert(&it->second);
				it->second.second.reset();
			}
			frozenTeam.erase(prev(frozenTeam.end()));
			rank.erase(jt);
			for(auto &p:t->status)
			{
				if(p.isFrozen())
				{
					int i=p.frozen->triesAfter;
					p.unfreeze();
					if(i)
						goto done;
				}
			}
			if(!frozenTeam.empty())
				goto retry;
		done:
			TeamRef cur=*t;
			jt=rank.insert(cur).first;
			if(t->hasFrozen())
				frozenTeam.insert({*t});
			if(nx&&nx<cur)
				os<<t->name<<' '<<prev(jt)->name<<' '<<t->accepted()<<' '<<t->penalty().t<<'\n';
			else if(pv&&cur<pv)
				os<<t->name<<' '<<next(jt)->name<<' '<<t->accepted()<<' '<<t->penalty().t<<'\n';
		}
		flush();
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