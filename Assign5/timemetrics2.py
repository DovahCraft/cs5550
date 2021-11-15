import numpy as np


shitcodeBF = [347.292239, 350.945302, 346.863862]
print(np.mean(shitcodeBF))
print(len(shitcodeBF))



O3code = [21.536613,21.482563,21.531244]
print(np.mean(O3code))
print(len(O3code))

#shitcodeBFOmp


bfOmp = [124.043543,125.279506,122.608346]
print(np.mean(bfOmp))

bfOmpO3 = [13.135677,13.342474,13.564788]
print(np.mean(bfOmpO3))


chadSequential = [0.067581, 0.061914,0.067798]
print("Chad sequential: ", np.mean(chadSequential))

chadSequentialO3 = [0.012887, 0.013030,0.012778]
print("Chad sequentialO3: ", np.mean(chadSequentialO3))


chadParallel = [0.019157, 0.023363, 0.021550]
print("Chad parallel: ", np.mean(chadParallel))

chadParallelO3 = [0.009398, 0.010713, 0.011587]
print("Chad parallelO3: ", np.mean(chadParallelO3))