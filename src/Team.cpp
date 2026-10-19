#include"forward.h"
#include<algorithm>

namespace reverSilly::ICPC
{
	Time Team::penalty()const
	{
		int res=0;
		for(auto &i:status)
			res+=i.penalty().t;
		return {res};
	}
	int Team::accepted()const
	{
		int res=0;
		for(auto &i:status)
			if(i.isAccepted())
				++res;
		return res;
	}
	void Team::add(Submission s)
	{
		status[s.problem->id].add(s);
	}
	std::strong_ordering operator<=>(Team const &a,Team const &b)
	{
		int aca=a.accepted(),acb=b.accepted();
		if(aca!=acb)
			return aca<=>acb;
		int pena=a.penalty().t,penb=b.penalty().t;
		if(pena!=penb)
			return pena<=>penb;
		std::vector<int>pensa,pensb;
		pensa.reserve(a.status.size());
		pensb.reserve(b.status.size());
		for(auto &s:a.status)
			pensa.push_back(s.penalty().t);
		for(auto &s:b.status)
			pensb.push_back(s.penalty().t);
		std::sort(pensa.rbegin(),pensa.rend());
		std::sort(pensb.rbegin(),pensb.rend());
		auto res=(pensa<=>pensb);
		if(res!=std::strong_ordering::equal)
			return res;
		return a.name<=>b.name;
	}
}