// A simple program that computes the square root of a number
// A simple program that computes the square root of a number
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>

#include "NPTrackingTools.h"


//Get Information about cameras: number of cameras, their position and orientation

int CamerasInfo()
{
	int numberOfCameras = TT_CameraCount();
	float cameraOrientation[9];
	float xCameraPosition, yCameraPosition, zCameraPosition;
	for (int i = 0; i < numberOfCameras; i++)
	{
		TT_SetCameraSettings(i, 2,1, 200, 15);
		
		xCameraPosition = TT_CameraXLocation(i);
		yCameraPosition = TT_CameraYLocation(i);
		zCameraPosition = TT_CameraZLocation(i);

		fprintf(stdout, "Position of the camera %d is %f, %f, %f \n", i, xCameraPosition, yCameraPosition, zCameraPosition);
		for (int j = 0; j < 9; j++){
			cameraOrientation[j] = TT_CameraOrientationMatrix(i, j);
		}
		fprintf(stdout, "Orientation of the camera %d is %f,%f,%f,%f,%f,%f,%f,%f,%f\n", i, cameraOrientation[0], cameraOrientation[1], cameraOrientation[2], cameraOrientation[3], cameraOrientation[4], cameraOrientation[5], cameraOrientation[6], cameraOrientation[7], cameraOrientation[8]);
	}
	return numberOfCameras;
}

// Create trackable using tool definition file (.txt). TO-DO: change to xml format

int CreateTrackableByFileName(std::string nameFile)
{
	FILE* calib_file;
	// Open the file
	try
	{
		calib_file = fopen(nameFile.c_str(), "r");
	}
	catch (std::ios_base::failure &fail)
	{
		fprintf(stdout, "[TestOptitrack]: Opening the file failed!");
	}

	int TrackableID = 0; // Trackable ID, we're just validating one
	
	// Get the name
	std::string m_ToolName = "";
	char* aux = new char[200];
	int resultFscan = fscanf(calib_file, "%s\n", aux);
	m_ToolName.append(aux);
	delete aux;

	fprintf(stdout, "[TestOptitrack]: Toolname: %s\n", m_ToolName.c_str());

	int numMarkers = 0;
	// Get the number of of points
	resultFscan = fscanf(calib_file, "%i\n", &numMarkers);
	

	fprintf(stdout, "[TestOptitrack]: Number of markers: %i \n", numMarkers);

	// Read the Calibration Point locations and save them
	float *calibrationPoints = new float[3 * numMarkers];

	
	for (int i = 0; i < numMarkers; i++)
	{
		resultFscan = fscanf(calib_file, "%fe", &calibrationPoints[i * 3 + 0]);

		if (resultFscan < 1)
		{
			fprintf(stdout, "[TestOptitrack]: Cannot read X location for marker \n");
			return 1;
		}

		resultFscan = fscanf(calib_file, "%f", &calibrationPoints[i * 3 + 1]);
		if (resultFscan < 1)
		{
			fprintf(stdout, "[TestOptitrack]: Cannot read Y location for marker \n");
			return 1;
		}

		resultFscan = fscanf(calib_file, "%f\n", &calibrationPoints[i * 3 + 2]);
		if (resultFscan < 1)
		{
			fprintf(stdout, "[TestOptitrack]: Cannot read Z location for marker \n");
			return 1;
		}

		calibrationPoints[i * 3 + 0] = calibrationPoints[i * 3 + 0] / 1000;
		calibrationPoints[i * 3 + 1] = calibrationPoints[i * 3 + 1] / 1000;
		calibrationPoints[i * 3 + 2] = -calibrationPoints[i * 3 + 2] / 1000;// Optitrack works with Left Handed System

	}

	// Read the Pivot Point location
	float *pivotPoint = new float[3];
	resultFscan = fscanf(calib_file, "%fe ", &pivotPoint[0]);
	if (resultFscan < 1)
	{
		fprintf(stdout, "[TestOptitrack]: Cannot read X location for Pivot Point \n");
		return 1;
	}

	resultFscan = fscanf(calib_file, "%fe ", &pivotPoint[1]);
	if (resultFscan < 1)
	{
		fprintf(stdout, "[TestOptitrack]: Cannot read Y location for Pivot Point \n");
		return 1;
	}

	resultFscan = fscanf(calib_file, "%fe\n", &pivotPoint[2]);
	if (resultFscan < 1)
	{
		fprintf(stdout, "[TestOptitrack]: Cannot read Z location for Pivot Point \n");
		return 1;
	}

	fprintf(stdout, "[TestOptitrack] \tPivotPoint \n");
	fprintf(stdout, "[TestOptitrack]: PivotPoint: %f, %f, %f \n", pivotPoint[0], pivotPoint[1], pivotPoint[2]);

	// mm -> m
	pivotPoint[0] = pivotPoint[0] / 1000;
	pivotPoint[1] = pivotPoint[1] / 1000;
	pivotPoint[2] = -pivotPoint[2] / 1000;

	// Create the Trackable


	NPRESULT resultCreateTrackable = TT_CreateTrackable(m_ToolName.c_str(), TrackableID, numMarkers, calibrationPoints);
		if (NPRESULT_SUCCESS == resultCreateTrackable)
		{
			fprintf(stdout, "[TestOptitrack]: Trackable Created Successfully \n");

	
			// Get marker positions according to configuration file (that is, the calibrated tool)
			float *CalibrationMarkerX = new float[numMarkers];
			float *CalibrationMarkerY = new float[numMarkers];
			float *CalibrationMarkerZ = new float[numMarkers];

			for (int i = 0; i < numMarkers; i++)
			{
				CalibrationMarkerX[i] = 0;
				CalibrationMarkerY[i] = 0;
				CalibrationMarkerZ[i] = 0;
				TT_TrackableMarker(TrackableID, i, &CalibrationMarkerX[i], &CalibrationMarkerY[i], &CalibrationMarkerZ[i]);
				CalibrationMarkerX[i] = CalibrationMarkerX[i] * 1000;
				CalibrationMarkerY[i] = CalibrationMarkerY[i] * 1000;
				CalibrationMarkerZ[i] = CalibrationMarkerZ[i] * 1000;
				fprintf(stdout, "Calibration position of the marker %i is %f, %f, %f \n", i, CalibrationMarkerX[i], CalibrationMarkerY[i], CalibrationMarkerZ[i]);
			}

			//Calculate distance values between calibrated markers
			for (int i = 0; i < numMarkers; i++)
			{
				for (int j = 0; j < numMarkers; j++)
				{
					if (i < j)
					{
						float CaldX = CalibrationMarkerX[i] - CalibrationMarkerX[j];
						float CaldY = CalibrationMarkerY[i] - CalibrationMarkerY[j];
						float CaldZ = CalibrationMarkerZ[i] - CalibrationMarkerZ[j];

						float CalDistance = sqrt((CaldX*CaldX) + (CaldY*CaldY) + (CaldZ*CaldZ));
						fprintf(stdout, "Distance between Calibrated markers %i and %i is %f \n", i, j, CalDistance);
					}
				}
			}

			return NPRESULT_SUCCESS;
			
		}
		else
		{
			fprintf(stdout, "[TestOptitrack]: Trackable Created Unsuccessfully \n");
			return NPRESULT_FAILED;
		}
	
}


// Track tool markers. For this, we are going to do the average X,Y,Z position of all the markers, out of 2000 samples

int TrackTrackableMarkers()
{
	NPRESULT resultUpdate;

	float *PointCloudMarkerAvX = new float[TT_TrackableMarkerCount(0)];
	float *PointCloudMarkerAvY = new float[TT_TrackableMarkerCount(0)];
	float *PointCloudMarkerAvZ = new float[TT_TrackableMarkerCount(0)];
	
	float *FrameMarkerAvX, *FrameMarkerAvY, *FrameMarkerAvZ;

	int MarkerCount = 0;
	int numSamples = 0;

	//Average X,Y,Z, Mx, My, Mz using a number of samples (numsamples)
	for (int count = 0; count < 1000; count++)
	{
		resultUpdate = TT_Update();

		//Only read frame markers if trackable is tracked
		if (TT_IsTrackableTracked(0))
		{
			for (int k = 0; k < TT_TrackableMarkerCount(0); k++){
				bool Tracked;
				float mX, mY, mZ;
				TT_TrackablePointCloudMarker(0, k, Tracked, mX, mY, mZ);
				if (numSamples == 0)
				{
					PointCloudMarkerAvX[k] = mX;
					PointCloudMarkerAvY[k] = mY;
					PointCloudMarkerAvZ[k] = mZ;
				}
				else
				{
					PointCloudMarkerAvX[k] += mX;
					PointCloudMarkerAvY[k] += mY;
					PointCloudMarkerAvZ[k] += mZ;
				}

			}


			// Here we read the number of markers in each frame, and the 3D position of the markers (position with reference to the whole tracking system)
			MarkerCount = TT_FrameMarkerCount();
			if (numSamples == 0)
			{
				FrameMarkerAvX = new float[MarkerCount];
				FrameMarkerAvY = new float[MarkerCount];
				FrameMarkerAvZ = new float[MarkerCount];
			}

			for (int l = 0; l < MarkerCount; l++)
			{
				if (numSamples == 0)
				{
					FrameMarkerAvX[l] = TT_FrameMarkerX(l) * 1000;
					FrameMarkerAvY[l] = TT_FrameMarkerY(l) * 1000;
					FrameMarkerAvZ[l] = TT_FrameMarkerZ(l) * 1000;
				}
				else
				{
					FrameMarkerAvX[l] += TT_FrameMarkerX(l) * 1000;
					FrameMarkerAvY[l] += TT_FrameMarkerY(l) * 1000;
					FrameMarkerAvZ[l] += TT_FrameMarkerZ(l) * 1000;
				}
			}

			//numSamples grows when TT_TrackableIsTracked
			numSamples++;

		}
	}

	//After sampling, divide AvX / numsamples to get average values

	for (int l = 0; l < TT_TrackableMarkerCount(0); l++)
	{
		PointCloudMarkerAvX[l] += PointCloudMarkerAvX[l] / numSamples;
		PointCloudMarkerAvY[l] += PointCloudMarkerAvY[l] / numSamples;
		PointCloudMarkerAvZ[l] += PointCloudMarkerAvZ[l] / numSamples;
		fprintf(stdout, "Point cloud marker position of marker %i is %f, %f, %f \n", l, PointCloudMarkerAvX[l], PointCloudMarkerAvY[l], PointCloudMarkerAvZ[l]);
		int label = TT_FrameMarkerLabel(l);      //== Returns Label of Marker -------
		fprintf(stdout, "Label of marker %i is %i \n", l, label);
	}

	for (int l = 0; l < MarkerCount; l++)
	{
		FrameMarkerAvX[l] = FrameMarkerAvX[l] / numSamples;
		FrameMarkerAvY[l] = FrameMarkerAvY[l] / numSamples;
		FrameMarkerAvZ[l] = FrameMarkerAvZ[l] / numSamples;
		fprintf(stdout, "3D average position of marker %i is %f, %f, %f \n", l, FrameMarkerAvX[l], FrameMarkerAvY[l], FrameMarkerAvZ[l]);
		int label = TT_FrameMarkerLabel(l);      //== Returns Label of Marker -------
		fprintf(stdout, "Label of marker %i is %i \n", l, label);
	}


	//Calculate marker-to-marker distance values

	for (int l = 0; l < TT_TrackableMarkerCount(0); l++)
	{
		for (int j = 0; j < TT_TrackableMarkerCount(0); j++)
		{
			if (l < j)
			{
				float PointClouddX = PointCloudMarkerAvX[l] - PointCloudMarkerAvX[j];
				float PointClouddY = PointCloudMarkerAvY[l] - PointCloudMarkerAvY[j];
				float PointClouddZ = PointCloudMarkerAvZ[l] - PointCloudMarkerAvZ[j];
				float PointCloudDistance = sqrt((PointClouddX*PointClouddX) + (PointClouddY*PointClouddY) + (PointClouddZ*PointClouddZ));
				fprintf(stdout, "Distance between Pointcloud markers %i and %i is %f \n", l, j, PointCloudDistance);
			}
		}
	}

	for (int l = 0; l < MarkerCount; l++)
	{
		for (int j = 0; j < MarkerCount; j++)
		{
			if (l < j)
			{
				float FramedX = FrameMarkerAvX[l] - FrameMarkerAvX[j];
				float FramedY = FrameMarkerAvY[l] - FrameMarkerAvY[j];
				float FramedZ = FrameMarkerAvZ[l] - FrameMarkerAvZ[j];
				float FrameDistance = sqrt((FramedX*FramedX) + (FramedY*FramedY) + (FramedZ*FramedZ));
				fprintf(stdout, "Distance between Frame markers %i and %i is %f \n", l, j, FrameDistance);

			}
		}
	}

	return resultUpdate;
}

int main (int argc, char *argv[])
{
  fprintf(stdout, "[TestOptitrack]: Calling TT_Initialize\n");

  NPRESULT result = TT_Initialize();
  
  fprintf(stdout, "[TestOptitrack]: End of TT_Initialize, result %i \n",result);

  NPRESULT resultCalibration = TT_LoadCalibration("J:/Calibration13022015.cal");
  fprintf(stdout, "[TestOptitrack]: End of Load Calibration, result %i \n", resultCalibration);


  int NumOfCameras = CamerasInfo();

  TT_ClearTrackableList();

  fprintf(stdout, "[TestOptitrack]: End of Clear trackable list \n");

  CreateTrackableByFileName("J:/RigidBody.txt");

  fprintf(stdout, "[TestOptitrack]: End of creating trackable \n");

  TrackTrackableMarkers();

  fprintf(stdout, "[TestOptitrack]: End of tracking \n");


  TT_RemoveTrackable(0);


  int end =  TT_Shutdown();

  
  return 0;
}