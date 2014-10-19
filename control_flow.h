#ifndef _CFG_H
#define _CFG_H
#include <vector>
#include <sstream>
#include <iostream>
#include<stack>
#include<string>
#include <fstream>
#include<set>

typedef struct BASIC_BLOCK
{
	std::string BlockLabel;
	std::string leader;
	std::vector<std::string> content;
	std::string end;
	std::pair<int,int> nextBlockIndex;
}
BasicBlock;

std::string ToString(int number)
{
	std::stringstream stream;
	stream << number;
	std::string s = "";
	stream >> s;
	return s;
}

void Control_Flow_Graph(std::string fileName, std::vector<BasicBlock>& blocks)
{
	unsigned int l_no = 0;
	std::string Case1 = " ?:= %s %s";
	std::string Case2 = " := %s";
	std::string Case3 = " : %s";
	std::ifstream input;
	input.open(fileName.c_str());
	BasicBlock c_Block;
	std::string line="";
	getline(input, line);
	l_no++;
	int found = -1;		
	while(found != 2)
	{
	  getline(input, line);
	  found = line.find("START");
	  l_no++; 
	}
	c_Block.leader = line;
	unsigned char s[100];
	int firstt = sscanf(line.c_str(), Case3.c_str(), s);
	if(firstt==1)
		c_Block.BlockLabel = std::string((const char*) s);
	else
		c_Block.BlockLabel = ToString(l_no);
	std::string previous = line;
	bool put_Leader = false;
	while(getline(input, line))
	{
		
		unsigned char a1[100];
		unsigned char a2[100];
		int t1 = sscanf(line.c_str(), Case1.c_str(), a1,a2);	
		t1 = sscanf(line.c_str(), Case1.c_str(), a1,a2);	
		int t2 = sscanf(line.c_str(), Case2.c_str(),a1);
		int t3 = sscanf(line.c_str(), Case3.c_str(), a1);
		if(t1==2)
		{
			put_Leader=true;
			c_Block.content.push_back(line);
			previous = line;
		}
		else if(t2==1)
		{
			put_Leader=true;
			c_Block.content.push_back(line);
			previous = line;
		}
		else if(t3==1 || put_Leader==true)
		{	if(!c_Block.content.empty())
			{				
				c_Block.end = c_Block.content[c_Block.content.size()-1];
				c_Block.content.pop_back();
			}			
			put_Leader = false;
			blocks.push_back(c_Block);
			BasicBlock newBlock;
			newBlock.content=std::vector<std::string>();
			newBlock.leader = "";
			newBlock.end="";
			c_Block = newBlock;
			c_Block.leader = line;
			if(t3 == 1)
				c_Block.BlockLabel = std::string((const char*) a1);
			else 
				c_Block.BlockLabel = ToString(l_no);
			previous = line;
		}
		else
		{
			c_Block.content.push_back(line);
			previous = line;
		}
		l_no++;
	}
	c_Block.end = c_Block.content[c_Block.content.size()-1];
	c_Block.content.pop_back();
	blocks.push_back(c_Block);
	for(int i = 0; i < blocks.size(); i++)
	{
		bool isJump = false;
		bool isPredicateJump = false;
		unsigned char v1[100];
		unsigned char v2[100];
		unsigned char v3[100];
		if(sscanf(blocks[i].end.c_str(), " ?:= %s %s", v1,v2)==2)
			isPredicateJump = true;
		if(sscanf(blocks[i].end.c_str(), " := %s", v3)==1)
			isJump=true;
		
		std::string a1 = std::string((const char*) v1);
		std::string a2 = std::string((const char*) v2);
		std::string value3 = std::string((const char*) v3);
		if(isJump == true)
		{
			for(int j = 0; j < blocks.size(); j++)
			{
				if(value3==blocks[j].BlockLabel)
				{
					blocks[i].nextBlockIndex.second = j;
					blocks[i].nextBlockIndex.first = j;
					break;
				}
			}
		}
		else if(isPredicateJump == true)
		{
			blocks[i].nextBlockIndex.first = i+1;
			for(int j = 0; j < blocks.size(); j++)
			{
				if(a1.substr(0,a1.length()-1)
					==blocks[j].BlockLabel)
				{
					blocks[i].nextBlockIndex.second = j;
					
					break;
				}
			}
		}
		else
		{
			blocks[i].nextBlockIndex.first=i+1;
			blocks[i].nextBlockIndex.second=i+1;
		}
	}
}


void printCFGGraph(std::string fileName, std::vector<BasicBlock>& blocks)
{
	std::ofstream output;
	output.open(fileName.c_str());
	output << "digraph CFGt {\n";
	for(int i = 0; i < blocks.size(); i++)
	{
		output << i << " [label=\"" << i << "\"];\n";
	}
	for(int i = 0; i < blocks.size(); i++)
	{
		output << i << " -> " << blocks[i].nextBlockIndex.first << ";\n";
		output << i << " -> " << blocks[i].nextBlockIndex.second << ";\n";
	}

	output << "}";

	output.close();
}
void pred(int n, std::vector<BasicBlock>& cfg, std::vector<int>& p)
{
	std::set<int> predessors = std::set<int>();
	for(int i = 0; i < cfg.size(); i++)
	{
		if(cfg[i].nextBlockIndex.first == n && predessors.find(i) == predessors.end())
		{
			predessors.insert(i);
			p.push_back(i);
		}
		if(cfg[i].nextBlockIndex.second==n && predessors.find(i)==predessors.end())
		{
			predessors.insert(i);
			p.push_back(i);
		}
	}
}
#endif
