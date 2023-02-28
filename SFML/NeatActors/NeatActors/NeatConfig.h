#pragma once
#include <array>

struct NeatConfig
{
public:
	static constexpr int epochs_per_batch = 5;

	//Network Structure Variables
	static constexpr int max_inputs = 2;
	static constexpr int max_outputs = 1;
	static constexpr int max_nodes = 10000;

	//Network Mutation Variables
	static constexpr float global_mutation_rate = 1.f;
	static constexpr float perturb_weight_rate = 0.16f * global_mutation_rate;
	static constexpr float weight_perturbation = 0.2f;
	static constexpr float scramble_weight_rate = 0.08f * global_mutation_rate;
	static constexpr float add_connection_rate = 0.2f * global_mutation_rate;
	static constexpr float add_node_rate = 0.1f * global_mutation_rate;
	static constexpr float disable_enable_connection_rate = 0.12f * global_mutation_rate;
	static constexpr float disable_enable_node_rate = 0.0f * global_mutation_rate;

	// Activation Functions: 0 = Sigmoid, 1 = Tanh, 2 = ReLU, 3 = Leaky ReLU, 4 = Linear, 5 = Gaussian, 6 = Sinusoid, 7 = Softplus, 8 = Absolute
	static std::array<int, 9> activation_weights;
	static constexpr int activation_weights_sum = 9;

	//Speciation Variables
	static float compatibility_threshold;
	static constexpr float mismatch_coefficient = 1.f;
	static constexpr float weight_coefficient = 0.5f;
	static constexpr int large_network_threshold = 20;

	//Natural Selection Variables
	static constexpr float cull_rate = 0.5f;
	static constexpr float inbreeding_percentage = 0.8f;
	static constexpr float mass_extinction_rate = 0.02f;
	static constexpr float mass_extinction_threshold = 0.25f;
};