#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <cmath>

using namespace std;
 
struct Chromosome
{
	int Bits[20];
	float Fitness;  
};

struct Adaptive_Fitness
{
	float Fitness_without_Penalty;
	float Obtained_Weight;
	float Core_Penalty;
	float Penalty_Multiplier;
	bool Valid_Solution;
};

float Set_Knapsack_Problem(float(&Weights)[20], float(&Values)[20], float Total_Weight, int seed)
{
	Total_Weight = 0;	
	for(int j = 0; j < 20; j++)
	{
		Weights[j] = rand() % 26 + 1;
		Values[j] = rand() % 51 + 1;
		Total_Weight = Total_Weight + Weights[j];
	}
	Total_Weight = Total_Weight * .4; 

	return Total_Weight;
}

void Set_Probability_Vector(float(&Probability_Vector)[20], int seed)
{
	for (int i = 0; i < 20; i++)
	{
		Probability_Vector[i] = 0.5; //Probability of bit being assigned one
	}
}

void Create_Population_from_Prob_Vector(float(&Probability_Vector)[20], Chromosome (&Solutions)[10], int seed)
{
	float Bit_Sampling;

	for(int i = 0; i < 10; i++)
	{
		for(int j = 0; j < 20; j++)
		{
			Bit_Sampling = rand() % 101;
			Bit_Sampling = Bit_Sampling/100;
			if (Bit_Sampling <= Probability_Vector[j])
			{			
				Solutions[i].Bits[j] = 1;
			}
			else
			{
				Solutions[i].Bits[j] = 0;
			}
		}
	}
}

void Bit_Mutation(Chromosome (&Solutions)[10])
{
	int Bit_Mutation;
	for(int i = 0; i < 10; i++)
	{
		for(int j = 0; j < 20; j++)
		{
			Bit_Mutation = rand() % 20;
			if (Bit_Mutation == 0)
			{			
				if(Solutions[i].Bits[j] == 1)
				{
					Solutions[i].Bits[j] = 0;
				}
				else
				{
					Solutions[i].Bits[j] = 1;
				}
			}
		}
	}	
}

void Calculate_Fitness(Adaptive_Fitness (&Knapsack_Fitness), Chromosome (&Solutions)[10], float(&Weights)[20], float(&Values)[20], float Total_Weight, int t)
{
	//float Core_Penalty[10];
	float Constant_Multi = 25; //Trail and Error

	for(int i = 0; i < 10; i++)
	{
		
		float Temporary_Values = 0;
		float Temporary_Weight = 0;		

		Knapsack_Fitness.Fitness_without_Penalty = 0;	
		Knapsack_Fitness.Obtained_Weight = 0;
		Knapsack_Fitness.Penalty_Multiplier = 0;
	
		
		//cout << "Generation " << t << endl;
		//cout << endl;
		for(int j = 0; j < 20; j++)
		{
			Temporary_Values = Solutions[i].Bits[j] * Values[j];
			Temporary_Weight = Solutions[i].Bits[j] * Weights[j];
			Knapsack_Fitness.Fitness_without_Penalty += Temporary_Values;
			Knapsack_Fitness.Obtained_Weight += Temporary_Weight;
		}

		Solutions[i].Fitness = Knapsack_Fitness.Fitness_without_Penalty;
		//cout << "Fitness no Penalty: " << Solutions[i].Fitness << endl;

		if(Knapsack_Fitness.Obtained_Weight <= Total_Weight)
		{
			Knapsack_Fitness.Core_Penalty = 0;
			//cout << "Valid Solution " << Knapsack_Fitness.Core_Penalty << endl;
		} 
		else
		{
			Knapsack_Fitness.Core_Penalty = Knapsack_Fitness.Obtained_Weight - Total_Weight;
			Knapsack_Fitness.Core_Penalty = Knapsack_Fitness.Core_Penalty + 1;
			Knapsack_Fitness.Core_Penalty = log(Knapsack_Fitness.Core_Penalty);
			//cout << "Invalid Solution " << Knapsack_Fitness.Core_Penalty <<  endl;
		}
		//Core_Penalty[i] = Knapsack_Fitness.Core_Penalty;
		//cout << "Core Penalty: " << Core_Penalty[i] << endl;

		Knapsack_Fitness.Penalty_Multiplier = Constant_Multi * (t + 1);
		//cout << "Penalty_Multiplier " << Knapsack_Fitness.Penalty_Multiplier <<  endl;

		
		Solutions[i].Fitness = Knapsack_Fitness.Fitness_without_Penalty - (Knapsack_Fitness.Penalty_Multiplier * Knapsack_Fitness.Core_Penalty);
		if(Solutions[i].Fitness < 0)
		{
			Solutions[i].Fitness = 0;
		}
		//cout << "Dynamic Fitness: " << Solutions[i].Fitness << endl;	
		//cout << endl;
	}
	//cout << endl;
}

void Transfer_Solutions_to_Temp(Chromosome (&Temp)[10], Chromosome (&Solutions)[10])
{
	for(int i = 0; i < 10; i++)
	{
		Temp[i] = Solutions[i];
	}
}


int Compare_Chromosomes(const void *s1, const void *s2)
{
	struct Chromosome *e1 = (struct Chromosome *)s1;
	struct Chromosome *e2 = (struct Chromosome *)s2;
	
	if (e1->Fitness < e2->Fitness) 
		return 1;
	else 
		return -1;	
}


void Find_Best_Solutions(Chromosome(&Temp)[10], Chromosome(&Best_Solution)[30], int t, int i)
{
	if(t == 0)
	{
		for(int k = 0; k < 20; k++)
		{
			Best_Solution[i].Bits[k] = 0;
		}
		Best_Solution[i].Fitness = 0;
	}
	
	if(Temp[0].Fitness >= Best_Solution[i].Fitness)
	{
		Best_Solution[i] = Temp[0];
	}
}

void Truncation(Chromosome (&Solutions)[10], Chromosome(&Temp)[10])
{
	int Truncation = 37;
	int T = Truncation%10;
	int Sampling;

	if (Truncation%10 > 0)
	{
		if(T < 5)
		{		
			Truncation = Truncation - T;
		}
		else
		{
			Truncation = Truncation + (10 - T);
		}
	}
	
	Truncation = Truncation/10;	
	
	for (int i = 0; i < 10; i++)
	{
		Sampling = rand() % (Truncation);
		Solutions[i] = Temp[Sampling];
	}
}

void Update_Probability_Vector(Chromosome (&Solutions)[10], float(&Probability_Vector)[20])
{
	float New_Probability_Vector[20];
	float Count = 0;
	float Lambda = 0.5;
	for(int i = 0; i < 20; i++)
	{
		for(int j = 0; j < 10; j++)
		{
			if(Solutions[j].Bits[i] == 1)
			{
				Count++;
			}
		}
		New_Probability_Vector[i] = Count/10;
		Count = 0;
	}
	
	for(int i = 0; i < 20; i++)
	{
		Probability_Vector[i] = Lambda * New_Probability_Vector[i] + (1 - Lambda) * Probability_Vector[i];
	}
}


void Print_Best_Solution(Chromosome(&Best_Solution)[30], int i)
{
	cout << "Best Chromosome of Run " << i + 1 << ":  ";
	for(int j = 0; j < 20; j++)
	{
		cout << Best_Solution[i].Bits[j];
	}
	cout << " Fitness: " <<  Best_Solution[i].Fitness << endl;	
}

void Statistics_Best_Run_Solutions(Chromosome(&Best_Solution)[30])
{
	float Average = 0;
	float Std_Deviation = 0;

	cout << endl;
	for(int i = 0; i < 30; i++)
	{
		 Average += Best_Solution[i].Fitness;
	}
	Average =  Average/30;
	cout << "Average Fitness of 30 Best of Runs: " << Average << endl;

	for(int i = 0; i < 30; i++)
	{
		Std_Deviation += pow((Average - Best_Solution[i].Fitness),2);
	}
	Std_Deviation = Std_Deviation/30;
	Std_Deviation = sqrt(Std_Deviation);
	cout << "Std Deviation Fitness of 30 Best of Runs: " << Std_Deviation << endl;

	cout << "Best solution of 30 Runs: ";	
	for(int j = 0; j < 20; j++)
	{
		cout << Best_Solution[0].Bits[j];
	}
	cout << " Fitness: " <<  Best_Solution[0].Fitness << " " << endl;	
}


/* These functions are all print functions used to check the intermiedate steps 
	between the Evolutionary Algorithm */
///////////////////////////////////////////////////////////////////////////////////
void Print_Probability_Vector(float(&Probability_Vector)[20])
{
	cout << "P(";	
	for (int i = 0; i < 20; i++)
	{
		if(i < 19)
		{
			cout << Probability_Vector[i] << ", ";
		}
		else
		{
			cout << Probability_Vector[i];
		}
	}
	cout << ")" << endl;
}

void Print_Population(Chromosome (&Solutions)[10])
{
	for(int i = 0; i < 10; i++)
	{
		cout << "Chromosome " << i << ": ";
		for(int j = 0; j < 20; j++)
		{
			cout << Solutions[i].Bits[j];
		}
		cout << " Fitness: " <<  Solutions[i].Fitness << endl;
	}
}

void Print_Temp_Population(Chromosome (&Temp)[10])
{
	for(int i = 0; i < 10; i++)
	{
		cout << "Chromosome " << i << ": ";
		for(int j = 0; j < 20; j++)
		{
			cout << Temp[i].Bits[j];
		}
		cout << " Fitness: " <<  Temp[i].Fitness << endl;
	}
}

void Print_Weights_Values_Problem(float(&Weights)[20], float(&Values)[20], float Total_Weight)
{
	cout << "This is the problem set up, organized in Objects, Weight of Object, Value of Object" << endl;
	cout << endl;	
		
	for(int j = 0; j < 20; j++)
	{
		cout << "Object: " <<j << " Weight: " << Weights[j] << " Values: " << Values[j] << endl; 		
	}
	cout << "Weight Limit: "<< Total_Weight << endl;
	cout << endl;	
}
///////////////////////////////////////////////////////////////////////////////////////////

int main()
{
	// Used for the Random function used throughout
	int seed = 7887;	
	srand(seed);	

	// Intializes Arrays
	float Probability_Vector[20];
	float Weights[20];
	float Values[20];

	// Intializes Total weight
	float Total_Weight = 0;

				
	// Intializes C structs
	Chromosome Solutions[10];
	Chromosome Temp[10];
	Chromosome Best_Solution[30];
	Adaptive_Fitness Knapsack_Fitness;

	// Sets up the Knapsack Problem
	Total_Weight = Set_Knapsack_Problem(Weights, Values, Total_Weight, seed);

	// Prints out the Stated Problem 	
	Print_Weights_Values_Problem(Weights, Values, Total_Weight);

	for(int i = 0; i < 30; i++)
	{
		// Used for the Random function used throughout 	
		int seed = 7887 + i;	
		srand(seed);		
	
		// Sets up the first Probability Vector, 50/50
		Set_Probability_Vector(Probability_Vector, seed);	
		
	
		// Intializes the zeroth Generation
		int t = 0;	
	
		do
		{
	
			// Creates Population from the input Probability Vector
			Create_Population_from_Prob_Vector(Probability_Vector, Solutions, seed);
	
			// Mutation occurs immediately the new population is created		
			Bit_Mutation(Solutions);
	
			// This is the adaptive penalty --- MUST KEEP TRACK OF FITNESS OF PREVIOUS GENERATION
			Calculate_Fitness(Knapsack_Fitness, Solutions, Weights, Values, Total_Weight, t);
	
			// Transfer Solutions for the truncation selection operator
			Transfer_Solutions_to_Temp(Temp, Solutions);
	
			// Sort the Temp solutions for truncation selection operator
			qsort(Temp, 10, sizeof(struct Chromosome), Compare_Chromosomes);
	
			// Function to find best solution in generation		
			Find_Best_Solutions(Temp, Best_Solution, t, i);
	
			/* Algorithm ends at Mutation, but Fitness must be calculated to complete the correct final
			vectors. I keep the Transfer Solutions function and qsort because the best solution 
			could be found in the last iteration */
			if(t < 29)
			{
				// TRUNCATION OPERATOR, output is the Solutions
				Truncation(Solutions, Temp);
		
				// Update Probability Vector from the Solutions struct
				Update_Probability_Vector(Solutions, Probability_Vector);
			}
	
						
			// Once a generation cycle is complete the counter goes up
			t++;
		
		}while(t < 30);
	
		// Prints the best solution in terminal
		Print_Best_Solution(Best_Solution, i);
	}
	// Sort the Best solutions, the solution index in Zero is the best solution of 30 Runs
	qsort(Best_Solution, 30, sizeof(struct Chromosome), Compare_Chromosomes);

	// Statistics of the Best individuals in the 30 Runs
	Statistics_Best_Run_Solutions(Best_Solution);

	return 0;
}
