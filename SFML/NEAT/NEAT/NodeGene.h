#pragma once
struct NodeGene
{
	int innovation_number;
	int xPos;

	
    bool operator<(const NodeGene& right) const
    {
        return xPos == right.xPos ? innovation_number < right.innovation_number : xPos < right.xPos;
    }
};