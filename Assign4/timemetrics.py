import numpy as np

x = [14.891785,14.842997,15.177252,15.218075,15.135996,15.102235,15.129945,15.165969,15.091224,15.144910]

print(np.mean(x))
print(len(x))

#Q2
thread1 = [2.352275,2.344274,2.347544,2.412014,2.359498,2.343156,2.353573,2.351171,2.347413,2.351379 ]
thread2 = [15.014517, 14.892804,14.855960,15.194149,14.902179,14.865518,14.850262,14.835116,14.848690,14.891036]
totalq2 = [15.014867, 14.894264,14.858457,15.199301,14.904699,14.868069,14.852780,14.837673,14.851117,14.894468]

thread1Mean = np.mean(thread1)
thread2Mean = np.mean(thread2)
totalMeanq2 = np.mean(totalq2)
loadImbalance = thread2Mean - thread1Mean

print("Thread1 mean", thread1Mean)
print("Thread2 mean", thread2Mean)
print("totalMeanq2", totalMeanq2)
print("Loadimbalance", loadImbalance)

#q3
print("\n===============\n q3\n===================\n")
thread1 = [8.797000,8.473169,8.460088,8.467231,8.576779,8.556745,8.565050,8.555227,8.598635,8.644824]
thread2 = [9.064998,8.756145,8.742298,8.731894,8.867537,8.818841,8.843382,8.839521,8.871436,8.946677]
totalq3 = [9.067919,8.758872,8.744959,8.734411,8.870060,8.821533,8.846237,8.842033,8.874044,8.949244]

thread1Mean = np.mean(thread1)
thread2Mean = np.mean(thread2)
totalMeanq3 = np.mean(totalq3)
loadImbalance = thread2Mean - thread1Mean

print(len(thread1))
print("Thread1 mean", thread1Mean)
print("Thread2 mean", thread2Mean)
print("totalMeanq2", totalMeanq3)
print("Loadimbalance", loadImbalance)


#q4
print("\n===============\n q4\n===================\n")
thread1 = [8.738479,8.741274,8.731513,8.732012,8.737240,8.754550,8.738267,8.757795,8.765441,8.737377]
thread2 = [8.459833,8.452304,8.450897,8.438918,8.460472,8.473404,8.454730,8.472206,8.483692,8.457524]
totalq4 = [8.740270,8.741575,8.731771,8.732344,8.737505,8.754928,8.738583,8.759443,8.767712,8.739043]
loadImbalance = [0.278646,0.288969,0.280616,0.293093,0.276769,0.281146,0.283537,0.285589,0.281749,0.279853]

thread1Mean = np.mean(thread1)
thread2Mean = np.mean(thread2)
totalMeanq4 = np.mean(totalq4)
loadImbalance = abs(thread2Mean - thread1Mean)
loadImbmean = np.mean(loadImbalance)

print(len(thread1))
print("Thread1 mean", thread1Mean)
print("Thread2 mean", thread2Mean)
print("totalMeanq2", totalMeanq4)
print("Loadimbalance", loadImbalance)
print("Loadimbalancemeeeansnasndfsa", loadImbmean)