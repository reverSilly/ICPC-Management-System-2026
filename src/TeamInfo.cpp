#include"forward.h"
#include<string_view>
#include<string>
#include<stdexcept>

namespace reverSilly::ICPC
{
	Team const*Contest::TeamInfo::teamByRank(int r)const
	{
		return rank.find_by_order(r)->t;
	}
	Team const*Contest::TeamInfo::teamByName(std::string const &sv)const
	{
		auto it=teams.find(sv);
		if(it!=teams.end())
			return &it->second.first;
		throw std::out_of_range{"cannot find the team"};
	}
	int Contest::TeamInfo::rankByTeam(Team const *t)const
	{
		return rank.order_of_key(TeamRef{t});
	}
	void Contest::TeamInfo::addTeam(Team const &t)
	{
		if(teams.contains(t.name))
			throw std::invalid_argument{"Add failed: duplicated team name."};
		auto it=teams.insert({t.name,{t,rank.begin()}}).first;
		auto [jt,_]=rank.insert({&it->second.first});
		it->second.second=jt;
		assert(_);
	}
	void Contest::TeamInfo::addProblem(Problem const *p)
	{
		for(auto &[k,v]:teams)
			v.first.status.emplace_back(p);
	}
	void Contest::TeamInfo::refresh(std::string const &s,std::function<void(Team&)>F,Set*sup)
	{
		auto &obj=teams.at(s);
		rank.erase({&obj.first});
		if(sup)
			sup->erase({&obj.first});
		F(obj.first);
		auto[it,_]=rank.insert({&obj.first});
		obj.second=it;
		assert(_);
		if(sup)
			sup->insert({&obj.first});
	}
}