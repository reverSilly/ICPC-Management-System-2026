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
	bool Team::hasFrozen()
	{
		for(auto &p:status)
		{
			if(p.isFrozen()&&p.frozen->triesAfter)
				return true;
			else if(p.isFrozen())
				p.unfreeze();
		}
		return false;
	}
	std::strong_ordering operator<=>(Team const &a,Team const &b)
	{
		if(&a==&b)
			return std::strong_ordering::equal;
		int aca=a.accepted(),acb=b.accepted();
		if(aca!=acb)
			return acb<=>aca;
		int pena=a.penalty().t,penb=b.penalty().t;
		if(pena!=penb)
			return pena<=>penb;
		std::vector<int>acta,actb;
		acta.reserve(aca);
		actb.reserve(acb);
		for(auto &s:a.status)
			if(s.isAccepted())
				acta.push_back(s.acceptedTime().t);
		for(auto &s:b.status)
			if(s.isAccepted())
				actb.push_back(s.acceptedTime().t);
		std::sort(acta.rbegin(),acta.rend());
		std::sort(actb.rbegin(),actb.rend());
		auto res=(acta<=>actb);
		if(res!=std::strong_ordering::equal)
			return res;
		return a.name<=>b.name;
	}
}