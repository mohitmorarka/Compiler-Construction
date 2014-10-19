#ifndef _Dominator_H_
#define _Dominator_H_
#include<map>
#include "control_flow.h"
void DFN(std::vector<BasicBlock>& cfg,int block,std::vector<bool>& visited,std::vector<int>& ordering,int& number)
{
	
	if(cfg[block].nextBlockIndex.first >= cfg.size()
	&& cfg[block].nextBlockIndex.second>= cfg.size())
	{
		if(visited[block]==false)
		{
		visited[block] =true;
		ordering[block] = number;
		number++;
		}
		return;
	}
	else
	{
		if(visited[block]==true)
			return;
		else
		{
			visited[block] =true;
			ordering[block] = number;
			number++;
		}
		if(cfg[block].nextBlockIndex.first < cfg.size())
		{
			DFN(cfg,
			cfg[block].nextBlockIndex.first,
			visited,
			ordering,
			number);			
		}
		if(cfg[block].nextBlockIndex.second < cfg.size())
		{
			DFN(cfg,
			cfg[block].nextBlockIndex.second,
			visited,
			ordering,
			number);
		}
	}
}

void Meet(std::set<int>& result,std::vector<std::set<int> >& sets)
{
	if(sets.size() == 1)
	{
		result = sets[0];

	}
	else if(sets.size() > 1)
	{
		std::set<int> s1 = sets[0];
		std::set<int> result2 = std::set<int>();
		for(int i = 1; i < sets.size(); i++)
		{
			std::set<int> s2 = sets[i];
			for(std::set<int>::iterator i = s1.begin();i != s1.end(); i++)
			{
				if(s2.find(*i)!= s2.end())
					result2.insert(*i);
			}
			s1 = result2;
			result2 = std::set<int>();
		}
		result = s1;
	}
}

bool compare(std::set<int>& s1, std::set<int>& s2)
{
	if(s1.size()==s2.size())
	{
		for(std::set<int>::iterator i = s1.begin();
			i != s1.end(); i++)
			if(s2.find(*i)==s2.end())
				return false;
		return true;
	}
	else
		return false;
}


void get_dominators(std::vector<BasicBlock>& cfg,std::map<int, std::set<int> >& dominators)
{
	dominators = std::map<int, std::set<int> >();
	dominators.insert(std::pair<int,std::set<int> >(0,std::set<int>()));
	dominators[0].insert(0);
	for(int i = 1; i < cfg.size(); i++)
	{
		dominators.insert(std::pair<int,std::set<int> >(i,std::set<int>()));
		for(int j = 0; j < cfg.size(); j++)
			dominators[i].insert(j);
	}

	bool change = true; 

	while(change == true)
	{

		change = false;
		std::set<int> set2 = std::set<int>();
		for(int i = 1; i < cfg.size(); i++)
		{
			std::set<int> intersection
				= std::set<int>();
			std::vector<std::set<int> > sets
				= std::vector<std::set<int> >();
			std::vector<int> p = std::vector<int>();
			pred(i, cfg, p);
			for(int j = 0; j < p.size(); j++)
				sets.push_back(dominators[p[j]]);

			Meet(intersection, sets);

			intersection.insert(i);

			if(compare(intersection,dominators[i])!= true)
				change = true;
			dominators[i] = intersection;
		}
	}
}

typedef struct LOOP
{
	std::set<int> ver;
}
Loop;

void getLoops(std::vector<Loop>& loops, std::vector<BasicBlock>& cfg)
{
	std::map<int, std::set<int> > dominators = std::map<int, std::set<int> >();
	
	get_dominators(cfg, dominators);

	
	std::vector<std::pair<int,int> > backEdges = std::vector<std::pair<int,int> >();
	std::vector<int> ordering = std::vector<int>();
	std::vector<int> ordering2 =std::vector<int>();
	
	std::vector<bool> visited = std::vector<bool>(cfg.size());
	ordering = std::vector<int>(cfg.size());
	int number = 0;
	int block = 0;
	DFN(cfg, block,visited,ordering,number);

	ordering2 = std::vector<int>(ordering.size());
	for(int i = 0; i < ordering.size(); i++)
		ordering2[ordering[i]]=i;
	
	for(int i = 0; i < ordering2.size(); i++)
	{
		int index = ordering2[i];
		int edge1 = cfg[index].nextBlockIndex.first;
		int edge2 = cfg[index].nextBlockIndex.second;

		if(dominators[index].find(edge1) != dominators[index].end())
			backEdges.push_back(std::pair<int,int>(index,edge1));
		
		if(edge1 != edge2 
		&& dominators[index].find(edge2)!= dominators[index].end())
			backEdges.push_back(std::pair<int,int>(index,edge2));
	}
	
	for(int i = 0; i < backEdges.size(); i++)
	{
		Loop loop;
		loop.ver = std::set<int>();
		std::stack<int> stack = std::stack<int>(); 
		loop.ver = std::set<int>();
		loop.ver.insert(backEdges[i].second);
		if(loop.ver.find(backEdges[i].first) == loop.ver.end())
		{
			loop.ver.insert(backEdges[i].first);
			stack.push(backEdges[i].first);
		}	
		while(stack.empty() != true)
		{
			int m = stack.top();
			stack.pop();
			std::vector<int> predessors = std::vector<int>();
			pred(m,cfg,predessors);
			for(int i = 0; i < predessors.size(); i++)
			{
				if(loop.ver.find(predessors[i]) == loop.ver.end())
				{
					loop.ver.insert(predessors[i]);
					stack.push(predessors[i]);
				}
			}
		}
		loops.push_back(loop);
	}
	
	
}

void printCFGBlocks(std::vector<BasicBlock>& cfg)
{
	for(int i = 0; i < cfg.size(); i++)
	{
		std::cout << "BB"<<i<<"\n";
		std::cout << cfg[i].leader << "\n";
		for(int j = 0; j < cfg[i].content.size(); j++)
			std::cout << cfg[i].content[j] << "\n";
		std::cout << cfg[i].end << "\n";
	}
}

void printEdges(std::vector<BasicBlock>& cfg)
{
	std::map<int,std::set<int> > edges = std::map<int,std::set<int> >();
	for(int i = 0; i < cfg.size(); i++)
		edges.insert(std::pair<int,std::set<int> >(i, std::set<int>()));
	
	for(int i = 0; i < cfg.size(); i++)
	{
		if(cfg[i].nextBlockIndex.first < cfg.size())
			edges[i].insert(cfg[i].nextBlockIndex.first);
		if(cfg[i].nextBlockIndex.second < cfg.size())
			edges[i].insert(cfg[i].nextBlockIndex.second);
	}

	
	int count = 0;
	for(int i = 0; i < cfg.size(); i++)
	{
		for(std::set<int>::iterator j = edges[i].begin(); j != edges[i].end(); j++)
		{
			std::cout << "EDGE " << count << ": BB" << i << " -> BB" 
					<< *j << "\n";
			count++;
		}
	}
}

void printLoops(std::vector<Loop>& loops)
{
	for(int i = 0; i < loops.size(); i++)
	{
		std::cout <<"LOOP " << i << ": ";
		for(std::set<int>::iterator j = loops[i].ver.begin();
		j != loops[i].ver.end(); j++)
		{
			std::cout << "BB" << *j << " ";
		}
		std::cout <<"\n";
	}
}
#endif
