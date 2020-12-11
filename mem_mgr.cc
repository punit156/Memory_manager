#include<iostream>
#include<fstream>
#include<string>
#include<algorithm>      
#include<vector>
#include<unordered_map>
using namespace std;
//Using struct to store data for all processes
struct processes{
	int start = -1;   //Starting address, initialized with -1
	int size = -1;   //Size
	bool active = false;  //Initialized with false, because not active
	int id;
};
//Check whether a given string is a number or not
int is_num(string check) 
{
	int ret = 1;
	int n = check.size();
	for(int i=0;i<n;i++)
	{
		int a = check[i];
		if(a<48 || a>57) //Not an integer
		{
			ret = 0;
			break;
		}
	}
	return ret;
}
int main(int argc,char *argv[])
{
	if(argc != 3) //Not 3 arguments, exit
	{
		cout<<"Not enough arguments. Program needs 3 arguments in the order 'executable input_file size'. Please try again"<<endl;
		return 0;
	}
	ifstream file(argv[1]);    //Opening the file specified as 1st argument
	int total_size = stoi(argv[2]);  //Converting total size from string to int
	string line;  
	int count = 0; // For counting number of lines in the file
	vector<string> input;
	while(getline(file,line))
	{
		input.push_back(line);   //Storing each line in a string array called 'input'
		count++;
	}
	struct processes process[count];  //Creating as many structs as there are number of lines
	int p = 0;  //Index for each process in struct
	for(int i=0;i<input.size();i++)  //Looping through the array of strings (lines of input file)
	{
		vector<string> command;
		int count = 0;
		if(input[i].size()==0) //blank line
		{
			continue;
		}
		for(int j=0;j<input[i].size();j++) //Parsing each line character by character
		{
			while(j<input[i].size() && (input[i][j]==' ' || input[i][j]=='\t')) // Space removal
			{
				j++;
			}
			string tmp = "";
			//Until a comment starts
			while(j<input[i].size() && input[i][j]!=' ' && input[i][j]!='\t' && input[i][j]!='/') 
			{
				tmp += input[i][j];
				j++;
			}
			command.push_back(tmp);
			count++;
			if(input[i][j] == '/')
			{
				break;
			}
		}//End of for
		if(command.size()==0) // Blank line
		{
			continue;
		}
		if(command.size()==1) // Invalid command
		{
			cout<<"Invalid command"<<endl;
			continue;
		}	
// Checking  whether the command is valid or not
		if(command[1]!="malloc" && command[1]!="free" && command[1]!="print")
		{
			cout<<"Wrong command"<<endl;
			continue;
		}
		else //Valid command
		{
			if(command[1] == "malloc") // Malloc
			{
				//Malloc command needs 3 arguments
				if(command.size() < 3)
				{
					cout<<"Incomplete command"<<endl;
					continue;
				}
				if(command.size()>3) //If >3 check whether 4th is a comment or not
				{
					if(command[3][0] != '/' && command[3].size()>0) //Not a comment
					{
						cout<<"Invalid Command"<<endl;
						continue;
					}
				}
				//Either 1st or 3rd argument are not numbers, then it's invalid
				if(!is_num(command[0]) || !is_num(command[2])) //Calling is_num()
				{
					cout<<"Either 1st or 3rd or both arguments are not number"<<endl;
					continue;
				}
				int id = stoi(command[0]); //Converting id from string to int
				vector<int> start;  //Array for all start times
				vector<int> holes_size;
				unordered_map<int,int> active;  //Hashmap of (start_location,size)
				unordered_map<int,int> holes; // holes(start,size)			
				int a = 0;
				while(a<p) // Looping over all active processes
				{
					//Check if a process is active or not
					if(!process[a].active) // Process terminated
					{
						a++;
						continue;
					}
				//Storing process size, indexed by process start location
					active[process[a].start] = process[a].size;
					start.push_back(process[a].start);
					a++;
				}
				sort(start.begin(),start.end()); //Sorting the start times
				int flag = 0;
				int biggest_hole = 0;
				int hole_start;
                        //Iterating over the processes again
				for(int i=0;i<start.size();i++)
				{
					flag = 1;
					int hole = 0;
					int index;
					if(i==0) // First element
					{
						if(start[0]!=0) //First process not starting at 0
						{
							hole = start[0];
							index = 0;
							if(hole > biggest_hole)
							{
								biggest_hole = hole;
								hole_start = index;
							}
							if(start.size()==1) // Just one element
							{
								hole = total_size - start[0] - active[start[0]];
								index = start[0] + active[start[0]];
							}
						}
						else // First process starting at 0
						{
							if(start.size()==1) // Only one process in memory
							{
								biggest_hole = total_size - active[start[0]];
								hole_start = active[start[0]];
								break;
							}
							else //More than one processes
							{
							//Second process not immediately after 1st
								if(start[1]!=active[start[0]])
								{
									hole = start[1] - active[start[0]] + 1;
									index = active[start[0]];
								}
							}
						}
					} //First element done
					else if(i==start.size()-1) // Last element
					{
						if(active[start[i-1]] + start[i-1] != start[i])
						{
							hole = start[i] - start[i-1] - active[start[i-1]];
							index = start[i-1] + active[start[i-1]];
							if(hole>biggest_hole)
							{
								biggest_hole = hole;
								hole_start = index;
							}
						}
						if(active[start[i]]+start[i]!=total_size) // Gap 
						{
							hole = total_size - active[start[i]] - start[i];
							index = active[start[i]] + start[i];
						}
					} //Last element done
					else // Middle elements
					{
						if(active[start[i-1]] + start[i-1] != start[i])
                                                {
                                                        hole = start[i] - start[i-1] - active[start[i-1]];
                                                        index = start[i-1] + active[start[i-1]];
						}
					}// Middle elements done
					if(hole>biggest_hole)
					{
						biggest_hole = hole;
						hole_start = index;
					}
				}// End of for, biggest hole found
				int space = stoi(command[2]);
				if(!flag) // No processes currently running, all space available
				{
					biggest_hole = total_size;
					hole_start = 0;
				}
				if(biggest_hole >= space) // Can Successfully allocate
				{
					cout<<"Allocated space : "<<space<<" for ID : "<<command[0]<<" starting from address : "<<hole_start<<endl;
					process[p].active = true;
					process[p].start = hole_start;
					process[p].size = space;
					process[p].id = stoi(command[0]);
					p++;
				}
				else // Can't allocate space
				{
					cout<<"Can't allocate "<<space<<" units of space. Not enough space"<<endl;
				}
			}//Malloc check done
			else if(command[1] == "free") //Free
			{
				if(command.size() < 3)
                                {
                                        cout<<"Incomplete command"<<endl;
                                        continue;
                                }
                                if(command.size()>3)
                                {
                                        if(command[3][0] != '/' && command[3].size()>0) //Not a comment
                                        {
                                                cout<<"Invalid Command"<<endl;
                                                continue;
                                        }
                                }
                                //Either 1st or 3rd argument are not numbers, then it's invalid
                                if(!is_num(command[0]) || !is_num(command[2]))
                                {
                                        cout<<"Either 1st or 3rd or both arguments are not number"<<endl;
                                        continue;
                                }
				int proc_id = stoi(command[2]);
				int flag = 0;
				for(int i=0;i<p;i++)
				{
					if(!process[i].active)  //Process terminated
					{
						continue;
					}
					else // Active
					{
						if(process[i].id == proc_id) //Process found for removal
						{
							cout<<"Removing process ID : "<<process[i].id<<" and freeing up "<<process[i].size<<" units of memory from location "<<process[i].start<<endl;
							flag = 1;
							process[i].active = false;
							process[i].start = -1;
							process[i].size = -1;
						}
					} //Active processes check done
				} //End of for
				if(!flag) // ID not found or doesn't match
				{
					cout<<"Inavlid ID : "<<proc_id<<endl;
				}	
			}//Free check done
			else if(command[1] == "print") // Print
			{
				if(command.size() < 2)
                                {
                                        cout<<"Incomplete command"<<endl;
                                        continue;
                                }
                                if(command.size()>2)
                                {
                                        if(command[2][0] != '/' && command[2].size()>0) //Not a comment
                                        {
                                                cout<<"Invalid Command"<<endl;
                                                continue;
                                        }
                                }
                                //Either 1st argument is not a number then it's invalid
                                if(!is_num(command[0]))
                                {
                                        cout<<"1st Argument is not a number"<<endl;
                                        continue;
                                }
				cout<<endl;
				cout<<"-------------------Print Memory Output----------------------"<<endl;
				unordered_map<int,int> active; //Active (start_addr,size)
				unordered_map<int,int> holes; // Hole (start_addr,size)
				vector<int> start;
				vector<int> empty;
				int flag = 0;
				int a = 0;
                                while(a<p) // Looping over all active processes
                                {
                                        if(!process[a].active) // Process terminated
                                        {
                                                a++;
                                                continue;
                                        }
					flag = 1;
                                        active[process[a].start] = process[a].size;
                                        start.push_back(process[a].start);
                                        a++;
                                }
				if(!flag) //No running processes, entire memory available
				{
					cout<<"Hole      start_address = 0 size = "<<total_size<<endl;
					cout<<"----------------------------------------------------------"<<endl;
					continue;
				}
				sort(start.begin(),start.end()); //Sort starting times
			//Finding unallocated spaces, holes
			for(int i=0;i<start.size();i++)
                                {
                                        int hole = 0;
                                        int index;
                                        if(i==0) // First element
                                        {
                                                if(start[0]!=0) //First process not starting at 0
                                                {
                                                        hole = start[0];
                                                        index = 0;
                                                	holes[index] = hole;
							empty.push_back(index);
                                                        if(start.size()==1) // Just one element
                                                        {
                                                                if(start[0]+active[start[0]]==total_size)
								{
									break;
								}
								else //Gap
								{
									hole = total_size - start[0] - active[start[0]];
									index = start[0] + active[start[0]];
									holes[index] = hole;
									empty.push_back(index);
									break;
								}
                                                        }
						}
						else // First process starting at 0
                                                {
                                                        if(start.size()==1) // Only one process in memory
                                                        {
                                                                hole = total_size - active[start[0]];
                                                                index = active[start[0]];
								holes[index] = hole;
								empty.push_back(index);
                                                                break;
                                                        }
						}
					}//First element done
					else if(i==start.size()-1) // Last element
                                        {
                                                if(active[start[i-1]] + start[i-1] != start[i])
                                                {
                                                        hole = start[i] - start[i-1] - active[start[i-1]];
                                                        index = start[i-1] + active[start[i-1]];
                                                        holes[index] = hole;
							empty.push_back(index);
                                                }
                                                if(active[start[i]]+start[i]!=total_size) // Gap
                                                {
                                                        hole = total_size - active[start[i]] - start[i];
                                                        index = active[start[i]] + start[i];
							holes[index] = hole;
                                                        empty.push_back(index);
                                                }
                                        } //Last element done
                                        else // Middle elements
                                        {
                                                if(active[start[i-1]] + start[i-1] != start[i])
                                                {
                                                        hole = start[i] - start[i-1] - active[start[i-1]];
                                                        index = start[i-1] + active[start[i-1]];
							holes[index] = hole;
                                                        empty.push_back(index);
                                                }
                                        }// Middle elements done
				}//End of for
			sort(empty.begin(),empty.end());
			int i=0,j=0;
			int m = start.size();
			int n = empty.size();
			while(i<m && j<n)
			{
				if(start[i]<empty[j]) //Allocated
				{
					cout<<"Allocated start_address = "<<start[i]<<" size = "<<active[start[i]]<<endl;
					i++;
				}
				else  //Hole
				{
					cout<<"Hole      start_address = "<<empty[j]<<" size = "<<holes[empty[j]]<<endl;
					j++;
				}
			}//End of while
			while(i<m)
			{
				cout<<"Allocated start_address = "<<start[i]<<" size = "<<active[start[i]]<<endl;
                                i++;
			}
			while(j<n)
			{
				 cout<<"Hole      start_address = "<<empty[j]<<" size = "<<holes[empty[j]]<<endl;
                                 j++;
			}
			cout<<"------------------------------------------------------------"<<endl;
			cout<<endl;
			}//Print check done
		}
	}
	file.close(); //Closing the input file
	return 0;
}
