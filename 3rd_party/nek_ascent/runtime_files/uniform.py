# state file generated using paraview version 5.12.0
import paraview
paraview.compatibility.major = 5
paraview.compatibility.minor = 12

#### import the simple module from the paraview
from paraview.simple import *
#### disable automatic camera reset on 'Show'
paraview.simple._DisableFirstRenderCameraReset()

# ----------------------------------------------------------------
# setup views used in the visualization
# ----------------------------------------------------------------

# Create a new 'Line Chart View'
lineChartView1 = CreateView('XYChartView')
lineChartView1.ViewSize = [1778, 1091]
lineChartView1.LeftAxisRangeMinimum = 0.1
lineChartView1.LeftAxisRangeMaximum = 0.68
lineChartView1.BottomAxisRangeMinimum = 0.075
lineChartView1.BottomAxisRangeMaximum = 0.20500000000000002
lineChartView1.RightAxisRangeMaximum = 6.66
lineChartView1.TopAxisRangeMaximum = 6.66

SetActiveView(None)

# ----------------------------------------------------------------
# setup view layouts
# ----------------------------------------------------------------

# create new layout object 'Layout #1'
layout1 = CreateLayout(name='Layout #1')
layout1.AssignView(0, lineChartView1)
layout1.SetSize(1778, 1091)

# ----------------------------------------------------------------
# restore active view
SetActiveView(lineChartView1)
# ----------------------------------------------------------------

# ----------------------------------------------------------------
# setup the data processing pipelines
# ----------------------------------------------------------------

# create a new 'Nek5000 Reader'
uniformnek5000 = Nek5000Reader(registrationName='uniform.nek5000', FileName='/home/siramok/Downloads/uniform.nek5000')
uniformnek5000.PointArrays = ['Velocity', 'Velocity Magnitude', 'Pressure']

# create a new 'Plot Over Line'
plotOverLine1 = PlotOverLine(registrationName='PlotOverLine1', Input=uniformnek5000)
plotOverLine1.Point1 = [0.0, 0.0, -0.1]
plotOverLine1.Point2 = [0.0, 0.0, 0.1]

# ----------------------------------------------------------------
# setup the visualization in view 'lineChartView1'
# ----------------------------------------------------------------

# show data from plotOverLine1
plotOverLine1Display = Show(plotOverLine1, lineChartView1, 'XYChartRepresentation')

# trace defaults for the display properties.
plotOverLine1Display.UseIndexForXAxis = 0
plotOverLine1Display.XArrayName = 'arc_length'
plotOverLine1Display.SeriesVisibility = ['Velocity Magnitude', 'Velocity_Magnitude']
plotOverLine1Display.SeriesLabel = ['arc_length', 'arc_length', 'Pressure', 'Pressure', 'Velocity_X', 'Velocity_X', 'Velocity_Y', 'Velocity_Y', 'Velocity_Z', 'Velocity_Z', 'Velocity_Magnitude', 'Velocity_Magnitude', 'Velocity Magnitude', 'Velocity Magnitude', 'vtkValidPointMask', 'vtkValidPointMask', 'Points_X', 'Points_X', 'Points_Y', 'Points_Y', 'Points_Z', 'Points_Z', 'Points_Magnitude', 'Points_Magnitude']
plotOverLine1Display.SeriesColor = ['arc_length', '0', '0', '0', 'Pressure', '0.8899977111467154', '0.10000762951094835', '0.1100022888532845', 'Velocity_X', '0.220004577706569', '0.4899977111467155', '0.7199969481956207', 'Velocity_Y', '0.30000762951094834', '0.6899977111467155', '0.2899977111467155', 'Velocity_Z', '0.6', '0.3100022888532845', '0.6399938963912413', 'Velocity_Magnitude', '1', '0.5000076295109483', '0', 'Velocity Magnitude', '0.6500038147554742', '0.3400015259021897', '0.16000610360875867', 'vtkValidPointMask', '0', '0', '0', 'Points_X', '0.8899977111467154', '0.10000762951094835', '0.1100022888532845', 'Points_Y', '0.220004577706569', '0.4899977111467155', '0.7199969481956207', 'Points_Z', '0.30000762951094834', '0.6899977111467155', '0.2899977111467155', 'Points_Magnitude', '0.6', '0.3100022888532845', '0.6399938963912413']
plotOverLine1Display.SeriesOpacity = ['arc_length', '1', 'Pressure', '1', 'Velocity_X', '1', 'Velocity_Y', '1', 'Velocity_Z', '1', 'Velocity_Magnitude', '1', 'Velocity Magnitude', '1', 'vtkValidPointMask', '1', 'Points_X', '1', 'Points_Y', '1', 'Points_Z', '1', 'Points_Magnitude', '1']
plotOverLine1Display.SeriesPlotCorner = ['Points_Magnitude', '0', 'Points_X', '0', 'Points_Y', '0', 'Points_Z', '0', 'Pressure', '0', 'Velocity Magnitude', '0', 'Velocity_Magnitude', '0', 'Velocity_X', '0', 'Velocity_Y', '0', 'Velocity_Z', '0', 'arc_length', '0', 'vtkValidPointMask', '0']
plotOverLine1Display.SeriesLabelPrefix = ''
plotOverLine1Display.SeriesLineStyle = ['Points_Magnitude', '1', 'Points_X', '1', 'Points_Y', '1', 'Points_Z', '1', 'Pressure', '1', 'Velocity Magnitude', '1', 'Velocity_Magnitude', '1', 'Velocity_X', '1', 'Velocity_Y', '1', 'Velocity_Z', '1', 'arc_length', '1', 'vtkValidPointMask', '1']
plotOverLine1Display.SeriesLineThickness = ['Points_Magnitude', '2', 'Points_X', '2', 'Points_Y', '2', 'Points_Z', '2', 'Pressure', '2', 'Velocity Magnitude', '2', 'Velocity_Magnitude', '2', 'Velocity_X', '2', 'Velocity_Y', '2', 'Velocity_Z', '2', 'arc_length', '2', 'vtkValidPointMask', '2']
plotOverLine1Display.SeriesMarkerStyle = ['Points_Magnitude', '0', 'Points_X', '0', 'Points_Y', '0', 'Points_Z', '0', 'Pressure', '0', 'Velocity Magnitude', '0', 'Velocity_Magnitude', '0', 'Velocity_X', '0', 'Velocity_Y', '0', 'Velocity_Z', '0', 'arc_length', '0', 'vtkValidPointMask', '0']
plotOverLine1Display.SeriesMarkerSize = ['Points_Magnitude', '4', 'Points_X', '4', 'Points_Y', '4', 'Points_Z', '4', 'Pressure', '4', 'Velocity Magnitude', '4', 'Velocity_Magnitude', '4', 'Velocity_X', '4', 'Velocity_Y', '4', 'Velocity_Z', '4', 'arc_length', '4', 'vtkValidPointMask', '4']

# ----------------------------------------------------------------
# setup animation scene, tracks and keyframes
# note: the Get..() functions create a new object, if needed
# ----------------------------------------------------------------

# get the time-keeper
timeKeeper1 = GetTimeKeeper()

# initialize the timekeeper

# get time animation track
timeAnimationCue1 = GetTimeTrack()

# initialize the animation track

# get animation scene
animationScene1 = GetAnimationScene()

# initialize the animation scene
animationScene1.ViewModules = lineChartView1
animationScene1.Cues = timeAnimationCue1
animationScene1.AnimationTime = 0.3222117226669
animationScene1.StartTime = 0.3222117226669
animationScene1.EndTime = 1.3222117226669

# initialize the animation scene

# ----------------------------------------------------------------
# restore active source
SetActiveSource(plotOverLine1)
# ----------------------------------------------------------------


##--------------------------------------------
## You may need to add some code at the end of this python script depending on your usage, eg:
#
## Render all views to see them appears
# RenderAllViews()
#
## Interact with the view, usefull when running from pvpython
# Interact()
#
## Save a screenshot of the active view
SaveScreenshot("summary.png")
#
## Save a screenshot of a layout (multiple splitted view)
# SaveScreenshot("path/to/screenshot.png", GetLayout())
#
## Save all "Extractors" from the pipeline browser
# SaveExtracts()
#
## Save a animation of the current active view
# SaveAnimation()
#
## Please refer to the documentation of paraview.simple
## https://kitware.github.io/paraview-docs/latest/python/paraview.simple.html
##--------------------------------------------
