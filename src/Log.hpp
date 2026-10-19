#pragma once
#include<iostream>

namespace reverSilly::ICPC
{
	template<typename T>
	void info(T const &m)
	{
		std::cout<<"[Info]"<<m<<'\n';
	}
	template<typename T>
	void warn(T const &m)
	{
		std::cout<<"[Warning]"<<m<<'\n';
	}
	template<typename T>
	void error(T const &m)
	{
		std::cout<<"[Error]"<<m<<'\n';
	}
}