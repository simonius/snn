#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define neuron_num 100

float rand_float(){
	return (float)rand()/(float)RAND_MAX;
}

struct neuron {
	float activation;
	float wghs[neuron_num];
};

void neuron_init(struct neuron *neuron)
{
	int i;
	neuron->activation = 0;
	for (i = 0; i < neuron_num;i++)
		neuron->wghs[i] = 0;

}

void net_init(struct neuron *net){
	int i;
	for (i = 0; i < neuron_num; i++)
		neuron_init(net+i);
}

float relu(float val)
{
	if (val > 0)
		return val;
	else
		return 0;
}

float input(struct neuron* neurons, int n)
{
	int i;
	float sum = 0;
	for (i = 0; i < n /*neuron_num*/; i++)
		sum += (neurons + i)->activation * (neurons + n)->wghs[i];
	return sum;
}

float calc_out(struct neuron *neurons, float *in, int values)
{
	int i;
	for (i = 0; i < values; i++)
		(neurons + i)->activation = *(in + i);
	for (; i < neuron_num; i++)
		(neurons + i)->activation = tanh(input(neurons, i));
	return neurons[neuron_num-1].activation;
}

void mutate_neuron(struct neuron *neuron, float delta)
{
	int i;
	for (i = 0; i < neuron_num; i++)
		if (rand_float() > 0.9){
			neuron->wghs[i] += (2*rand_float()-1)*delta;
	}
}

void mutate_net(struct neuron *neurons)
{
	int i;
	for (i = 0; i < neuron_num; i++)
		if(rand_float() > 0.9)
			mutate_neuron(neurons + i, 0.01);
}

void network_sex(struct neuron *net1, struct neuron *net2, struct neuron *child)
{
	int i, j;
	for (i = 0; i < neuron_num; i++){
		if (rand_float() > 0.5)
			(child+i)->activation = (net1+i)->activation;
		else
			(child+i)->activation = (net2+i)->activation;
		for (j = 0; j < neuron_num; j++) {
			if (rand_float() > 0.5)
				(child+i)->wghs[j] = (net1+i)->wghs[j];
			else
				(child+i)->wghs[j] = (net2+i)->wghs[j];
		}
	}

}


float score(struct neuron *net, float *input, int input_dim, int batchsize)
{
	int i;
	float value;
	float sum = 0;
	for (i = 0; i < batchsize; i++){
		value = calc_out(net,(input+(input_dim+1)*i), input_dim);
		sum += (value-input[(input_dim+1)*i-1])*(value-input[(input_dim+1)*i-1]);
	}
	return sum / batchsize;
}

void netcpy(struct neuron *rst, struct neuron *orig)
{
	network_sex(orig, orig, rst);
}

int gen_train(struct neuron *net, float *input, int input_dim, int batchsize)
{
	struct neuron nets[neuron_num][10];
	int i, j;
	float scores[10];
	float start;

	network_sex(net, net, nets[0]);
	network_sex(net, net, nets[1]);
	mutate_net(nets[1]);
	network_sex(net, net, nets[2]);
	mutate_net(nets[2]);

	while(1){
		network_sex(nets[0], nets[1], nets[3]);
		network_sex(nets[0], nets[2], nets[4]);
		network_sex(nets[1], nets[2], nets[5]);

		netcpy(nets[6], nets[3]);
		mutate_net(nets[6]);
		netcpy(nets[7], nets[4]);
		mutate_net(nets[7]);
		netcpy(nets[8], nets[5]);
		mutate_net(nets[8]);

		for(i = 0; i < 10; i++)
			scores[i] = score(nets[i], input, input_dim, batchsize);
		start = 3;
		i = 0;
		for(j = start; j < 10; j++) {
			if(scores[j] < scores[0]) {
				printf("new highscore 1: %f 2: %f 3: %f \n", scores[0], scores[1], scores[3]);
				netcpy(nets[i], nets[j]);
				scores[0] = scores[j];
		//		j = j + 1;
			}
			if(scores[j] < scores[1]) {
				printf("new highscore 1: %f 2: %f 3: %f \n", scores[0], scores[1], scores[2]);
				netcpy(nets[1], nets[j]);
				scores[1] = scores[j];
		//		j = j + 1;
			}
			if(scores[j] < scores[2]){
				printf("new highscore 1: %f 2: %f 3: %f \n", scores[0], scores[1], scores[2]);
				netcpy(nets[2], nets[j]);
				scores[2] = scores[j];
		//		j = j + 1;
			}
		}
	}

}

void train_data(float *vec)  /*XOR*/
{
	int i = 0;
	vec[0]=0;
	vec[1]=0;
	vec[2]=1;
	vec[3]=0;

	vec[4]=0;
	vec[5]=1;
	vec[6]=1;
	vec[7]=1;

	vec[8]=1;
	vec[9]=0;
	vec[10]=1;
	vec[11]=1;

	vec[12]=1;
	vec[13]=1;
	vec[14]=1;
	vec[15]=0;
}

int main(){
	struct neuron net[neuron_num];
	int i;
	float data[16];
	srand(1701);
	train_data(data);

	net_init(net);
	gen_train(net, data, 3, 4);
}


