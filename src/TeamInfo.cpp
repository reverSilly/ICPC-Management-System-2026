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
	Team const*Contest::TeamInfo::teamByName(std::string_view sv)const
	{
		auto it=teams.find(std::string(sv));
		if(it!=teams.end())
			return &it->second.first;
		return nullptr;
	}
	int Contest::TeamInfo::rankByTeam(Team const *t)const
	{
		return rank.order_of_key(TeamRef{t});
	}
	void Contest::TeamInfo::addTeam(Team const &t)
	{
		if(teams.contains(t.name))
			throw std::invalid_argument{"Team already exist"};
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
	void Contest::TeamInfo::refresh(Team const *team,std::function<void(Team&)> F)
	{
		auto &obj=teams.at(team->name);
		rank.erase({&obj.first});
		F(obj.first);
		auto[it,_]=rank.insert({&obj.first});
		obj.second=it;
		assert(_);
	}
}