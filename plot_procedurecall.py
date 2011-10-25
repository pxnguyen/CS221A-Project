from pylab import plot, show, xlabel, ylabel, title, grid

x = [i for i in range(-1,8)];
y = [7.501735, 9.082093, 9.615858, 11.452664, 13.848561, 14.903877, 17.632770, 18.727919, 23.459625]

plot(x,y)
xlabel("Parameter Count")
ylabel("Clock cycle")
title("Clock cycle as a function of parameter count")
grid(True)
show()
