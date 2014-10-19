#include "control_flow.h"
#include "Dominator.h"

void get_BasicBlocks(std::string fileName, std::vector<BasicBlock>& blocks)
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
		{			
			c_Block.end = c_Block.content[c_Block.content.size()-1];
			c_Block.content.pop_back();
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
}

int main(int argc, char** argv)
{

	if(argc > 1)
	{
		std::string fileName = std::string(argv[1]);
		std::vector<BasicBlock> blocks = std::vector<BasicBlock>();
		Control_Flow_Graph(fileName, blocks);
		std::vector<Loop> loops = std::vector<Loop>();
		getLoops(loops, blocks);
		std::map<int,std::set<int> > imm = std::map<int,std::set<int> >();
		std::map<int, std::set<int> > dominators 
			= std::map<int, std::set<int> >();
		get_dominators(blocks, dominators);
		printCFGGraph("control_flow.dot", blocks);
		std::cout << "\n\n";
		printCFGBlocks(blocks);
		printEdges(blocks);
		printLoops(loops);
	}
}



