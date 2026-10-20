#include"forward.h"
#include<string_view>
#include<string>
#include<stdexcept>

namespace reverSilly::ICPC
{
	TeamRef const& Contest::TeamInfo::teamByRank(int r)const
	{
		return *rank.find_by_order(r);
	}
	Contest::TeamInfo::Array::point_iterator Contest::TeamInfo::teamByName(std::string const &sv)
	{
		return teams.find(sv);
	}
	Contest::TeamInfo::Array::point_const_iterator Contest::TeamInfo::teamByName(std::string const &sv)const
	{
		return teams.find(sv);
	}
	int Contest::TeamInfo::rankByTeam(Array::point_const_iterator it)const
	{
		return rank.order_of_key(*it->second.second);
	}
	void Contest::TeamInfo::addTeam(Team const &t)
	{
		if(teams.find(t.name)!=teams.end())
			throw std::invalid_argument{"Add failed: duplicated team name."};
		dirty.insert(&teams.insert({t.name,{t,rank.end()}}).first->second);
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
	void Contest::TeamInfo::rebuild()
	{
		for(auto i:dirty)
			rank.erase(i->second);
		for(auto i:dirty)
			i->second=rank.insert(i->first).first;
		dirty.clear();
	}
}