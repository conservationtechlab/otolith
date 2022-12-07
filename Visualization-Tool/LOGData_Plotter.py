import matplotlib.pyplot as plt
#TODO: mke these functions more input argument based: allow users to define what gets plotted and on how many axes
    #COULD IMPLEMENT LOG CONSTANTS HERE !!!!! SO YOU CAN LIST WHICH TO PLOT!!!!!! (label data can be dotted lines over plots)

#TODO: figure out why labels aren't working

def plotAccelerationDataOnly(motion_data):
    plt.title('Acceleration')
    plt.plot(motion_data[:,0],motion_data[:,1], label = "ax")
    plt.plot(motion_data[:,0],motion_data[:,2], label = "ay")
    plt.plot(motion_data[:,0],motion_data[:,3], label = "az")
    plt.legend()

    plt.show()

def plotData_MultipleFigures(motion_data):
    plt.figure(1)
    plt.title('Acceleration')
    plt.plot(motion_data[:,0],motion_data[:,1], label = "ax")
    plt.plot(motion_data[:,0],motion_data[:,2], label = "ay")
    plt.plot(motion_data[:,0],motion_data[:,3], label = "az")

    plt.figure(2)
    plt.title('Angular Velocity')
    plt.plot(motion_data[:,0],motion_data[:,4], label = "wx")
    plt.plot(motion_data[:,0],motion_data[:,5], label = "wy")
    plt.plot(motion_data[:,0],motion_data[:,6], label = "wz")

    plt.figure(3)
    plt.title('Angle')
    plt.plot(motion_data[:,0],motion_data[:,7], label = "x°")
    plt.plot(motion_data[:,0],motion_data[:,8], label = "y°")
    plt.plot(motion_data[:,0],motion_data[:,9], label = "z°")

    plt.figure(4)
    plt.title('Magnetic Strength')
    plt.plot(motion_data[:,0],motion_data[:,11], label = "hx")
    plt.plot(motion_data[:,0],motion_data[:,12], label = "hy°")
    plt.plot(motion_data[:,0],motion_data[:,13], label = "hz°")

    plt.legend()

    plt.show()

def plotData_SubPlots(motion_data):
    fig, ax = plt.subplots(4, 1, sharex='col')
    accel_plot = ax[0]
    accel_plot.title.set_text('Acceleration')
    accel_plot.plot(motion_data[:,0],motion_data[:,1], label = "ax")
    accel_plot.plot(motion_data[:,0],motion_data[:,2], label = "ay")
    accel_plot.plot(motion_data[:,0],motion_data[:,3], label = "az")
    accel_plot.legend()

    angVel_plot = ax[1]
    angVel_plot.title.set_text('Angular Velocity')
    angVel_plot.plot(motion_data[:,0],motion_data[:,4], label = "wx")
    angVel_plot.plot(motion_data[:,0],motion_data[:,5], label = "wy")
    angVel_plot.plot(motion_data[:,0],motion_data[:,6], label = "wz")
    #angVel_plot.legend()

    angle_plot = ax[2]
    angle_plot.title.set_text('Angle')
    angle_plot.plot(motion_data[:,0],motion_data[:,7], label = "x°")
    angle_plot.plot(motion_data[:,0],motion_data[:,8], label = "y°")
    angle_plot.plot(motion_data[:,0],motion_data[:,9], label = "z°")
    #angle_plot.legend()

    magn_plot = ax[3]
    magn_plot.title.set_text('Magnetic Strength')
    magn_plot.plot(motion_data[:,0],motion_data[:,11], label = "hx")
    magn_plot.plot(motion_data[:,0],motion_data[:,12], label = "hy°")
    magn_plot.plot(motion_data[:,0],motion_data[:,13], label = "hz°")
    #magn_plot.legend()

    #handles, labels = ax.get_legend_handles_labels()
    #fig.legend(handles, labels, loc='upper center')

    plt.show()

def plotData_vs_Labels_SubPlots(motion_data,label_data):
    fig, ax = plt.subplots(4, 1, sharex='col')
    
    accel_max = max(motion_data[:,1])
    accel_plot = ax[0]
    accel_plot.title.set_text('Acceleration')
    accel_plot.plot(motion_data[:,0],motion_data[:,1], label = "x")
    accel_plot.plot(motion_data[:,0],motion_data[:,2], label = "y")
    accel_plot.plot(motion_data[:,0],motion_data[:,3], label = "z")
    accel_plot.plot(label_data[0],label_data[1]*accel_max, linestyle = '--', label = "standing")
    accel_plot.plot(label_data[0],label_data[2]*accel_max, linestyle = '--', label = "walking")
    accel_plot.legend()

    angVel_max = max(motion_data[:,4])
    angVel_plot = ax[1]
    angVel_plot.title.set_text('Angular Velocity')
    angVel_plot.plot(motion_data[:,0],motion_data[:,4], label = "wx")
    angVel_plot.plot(motion_data[:,0],motion_data[:,5], label = "wy")
    angVel_plot.plot(motion_data[:,0],motion_data[:,6], label = "wz")
    angVel_plot.plot(label_data[0],label_data[1]*angVel_max, linestyle = '--', label = "standing")
    angVel_plot.plot(label_data[0],label_data[2]*angVel_max, linestyle = '--', label = "walking")

    angle_max = max(motion_data[:,7])
    angle_plot = ax[2]
    angle_plot.title.set_text('Angle')
    angle_plot.plot(motion_data[:,0],motion_data[:,7], label = "x°")
    angle_plot.plot(motion_data[:,0],motion_data[:,8], label = "y°")
    angle_plot.plot(motion_data[:,0],motion_data[:,9], label = "z°")
    angle_plot.plot(label_data[0],label_data[1]*angle_max, linestyle = '--', label = "standing")
    angle_plot.plot(label_data[0],label_data[2]*angle_max, linestyle = '--', label = "walking")

    magn_max = max(motion_data[:,11])
    magn_plot = ax[3]
    magn_plot.title.set_text('Magnetic Strength')
    magn_plot.plot(motion_data[:,0],motion_data[:,11], label = "hx")
    magn_plot.plot(motion_data[:,0],motion_data[:,12], label = "hy°")
    magn_plot.plot(motion_data[:,0],motion_data[:,13], label = "hz°")
    magn_plot.plot(label_data[0],label_data[1]*magn_max, linestyle = '--', label = "standing")
    magn_plot.plot(label_data[0],label_data[2]*magn_max, linestyle = '--', label = "walking")


    plt.show()

def plotDataAndLabel_SubPlots(motion_data, label_data):
    fig, ax = plt.subplots(3, 1, sharex='col')
    accel_plot = ax[0]
    accel_plot.title.set_text('Acceleration')
    accel_plot.plot(motion_data[:,0],motion_data[:,1], label = "ax")
    accel_plot.plot(motion_data[:,0],motion_data[:,2], label = "ay")
    accel_plot.plot(motion_data[:,0],motion_data[:,3], label = "az")
    accel_plot.legend()

    angVel_plot = ax[1]
    angVel_plot.title.set_text('Angular Velocity')
    angVel_plot.plot(motion_data[:,0],motion_data[:,4], label = "wx")
    angVel_plot.plot(motion_data[:,0],motion_data[:,5], label = "wy")
    angVel_plot.plot(motion_data[:,0],motion_data[:,6], label = "wz")
    angVel_plot.legend()

    actv_plot = ax[2]
    actv_plot.title.set_text('Activity')
    actv_plot.plot(label_data[0],label_data[1], label = "standing")
    actv_plot.plot(label_data[0],label_data[2], linestyle = ':', label = "walking")
    actv_plot.legend()

    plt.show() #TODO: figure out a better way to put plotting functions in one function but run from another, maybe jut move main here