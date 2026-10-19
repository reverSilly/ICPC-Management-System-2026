#include"forward.h"
#include<format>
#include<cassert>
#include<stdexcept>

namespace reverSilly::ICPC
{
	std::string ProblemStatus::str()const
	{
		using std::format;
		if(frozen.value_or({0,0}).triesAfter!=0)
		{
			auto[bef,aft]=*frozen;
			if(bef==0)
				return format("0/{}",aft);
			return format("-{}/{}",bef,aft);
		}
		if(isAccepted())
		{
			if(tries)
				return format("+{}",tries);
			return "+";
		}
		else
		{
			if(tries)
				return format("-{}",tries);
			return ".";
		}
	}
	void ProblemStatus::freeze()
	{
		if(isFrozen())
			throw std::logic_error{"already frozen"};
		frozen={tries,0};
	}
	void ProblemStatus::unfreeze()
	{
		if(!isFrozen())
			throw std::logic_error{"not frozen"};
		frozen.reset();
	}
	bool ProblemStatus::isFrozen()const
	{
		return frozen.has_value();
	}
	void ProblemStatus::add(Submission s)
	{
		if(s.problem!=problem)
			throw std::invalid_argument("adding submission of other problem");
		if(!accepted)
		{
			if(s.isAccepted())
				accepted=true;
			else
				tries++;
		}
		submissions[static_cast<size_t>(s.status)].push_back(s);
	}
	bool ProblemStatus::isAccepted()const
	{
		if(isFrozen())
			return false;
		return accepted;
	}
	Time ProblemStatus::acceptedTime()const
	{
		if(isAccepted())
			return submissions[0].front().time;
		else
			throw std::logic_error{"N/A"};
	}
	Time ProblemStatus::penalty()const
	{
		if(!isAccepted())
			return {0};
		return {tries*20+acceptedTime().t};
	}
}