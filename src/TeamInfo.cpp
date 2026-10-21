#include"forward.h"
#include<string_view>
#include<string>
#include<stdexcept>

namespace reverSilly::ICPC
{
	Contest::TeamInfo::Array::iterator Contest::TeamInfo::teamByName(std::string const &sv)
	{
		return teams.find(sv);
	}
	Contest::TeamInfo::Array::const_iterator Contest::TeamInfo::teamByName(std::string const &sv)const
	{
		return teams.find(sv);
	}
	int Contest::TeamInfo::rankByTeam(Array::const_iterator it)const
	{
		return it->second.rank;
	}
	void Contest::TeamInfo::addTeam(Team const &t)
	{
		if(teams.find(t.name)!=teams.end())
			throw std::invalid_argument{"Add failed: duplicated team name."};
		teams.insert({t.name,{t,rank.end(),-1}});
	}
	void Contest::TeamInfo::addProblem(Problem const *p)
	{
		for(auto &[k,v]:teams)
			v.first.status.emplace_back(p);
	}
	void Contest::TeamInfo::refresh(std::string const &s,std::function<void(Team&)>F)
	{
		auto it=teams.find(s);
		if(it==teams.end())
			throw std::invalid_argument{"team not exist"};
		auto &obj=it->second;
		dirty.insert(&it->second);
		F(obj.first);
	}
	void Contest::TeamInfo::rerank()
	{
		int r=0;
		for(auto &i:rank)
			teams.find(std::string{i.name})->second.rank=r++;
	}
	void Contest::TeamInfo::rebuild()
	{
		for(auto i:dirty)
			if(i->second)
				rank.erase(*i->second);
		std::list<TeamRef>tmp;
		for(auto i:dirty)
			i->second=tmp.insert(tmp.end(),i->first);
		tmp.sort();
		rank.merge(std::move(tmp));
		dirty.clear();
		rerank();
	}
}