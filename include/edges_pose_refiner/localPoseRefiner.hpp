/*
 * localPoseRefiner.hpp
 *
 *  Created on: Apr 21, 2011
 *      Author: Ilya Lysenkov
 */

#ifndef LOCALPOSEREFINER_HPP_
#define LOCALPOSEREFINER_HPP_

#include <opencv2/core/core.hpp>

#include "edges_pose_refiner/edgeModel.hpp"
#include "edges_pose_refiner/utils.hpp"


/** \brief Parameters of pose refinement by local optimization */
struct LocalPoseRefinerParams
{
  /** \brief distance type in distance transorm of an edges map */
  int distanceType;

  /** \brief distance mask in distance transorm of an edges map */
  int distanceMask;

  //Levenberg-Marquardt parameters
  /** \brief downsampling factor to use when computing silhouette edgels */
  float lmDownFactor;

  /** \brief number of closing operations when computing silhouette edges */
  int lmClosingIterationsCount;

  /** \brief ratio of inliers when applying Levenberg-Marquardt */
  float lmInliersRatio;

  /** \brief compute silhouettes by morhology (true) or by using normals (false) */
  bool useAccurateSilhouettes;

  /** \brief termination criteria of Levenberg-Marquardt in pose refinement */
  cv::TermCriteria termCriteria;

  LocalPoseRefinerParams()
  {
    distanceType = CV_DIST_L2;
    distanceMask = CV_DIST_MASK_PRECISE;

    lmDownFactor = 1.0f;
    lmClosingIterationsCount = 10;
//    lmInliersRatio = 0.8f;
    lmInliersRatio = 0.65f;

    useAccurateSilhouettes = true;
    termCriteria = cv::TermCriteria(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 30, DBL_EPSILON);
  }
};

/** \brief Pose refinement by local optimization */
class LocalPoseRefiner
{
public:
  /** \brief Create a class instance which will refine poses of the specific object by local optimization
   *
   *  \param edgeModel The edge model of the object
   *  \param edgesImage Edges of the test images. They can be computed by Canny.
   *  \param cameraMatrix Intrinsic parameters of the test camera
   *  \param distCoeffs Distortion coefficients of the test camera
   *  \param extrinsicsRt Extrinsic parameters of the test camera
   *  \param params Parameters of local optimization
   */
  LocalPoseRefiner(const EdgeModel &edgeModel, const cv::Mat &edgesImage, const cv::Mat &cameraMatrix, const cv::Mat &distCoeffs, const cv::Mat &extrinsicsRt, const LocalPoseRefinerParams &params = LocalPoseRefinerParams());

  /** \brief Sets paremeters of pose refinement
   *
   *  \param params parameters of pose refinement
   */
  void setParams(const LocalPoseRefinerParams &params);

  /** \brief Set test edges for silhouette edgels
   * \param edges silhouette test edges
   */
  void setSilhouetteEdges(const cv::Mat &edges);

  /** \brief Refine pose of the object using silhouette edges also
   *
   *  \param pose_cam The refined pose (in the test camera coordinate system)
   *  \param usePoseGuess If true the function will use provided rvec and tvec as the initial
approximations of the rotation and translation vectors, respectively, and will further optimize
them. Else the function will use zero rotation and translation as initial guess.
   *  \param tablePlane coefficients of a table plane equation which is used as additional constraint
   *  \return registration error
   */
  float refineUsingSilhouette(PoseRT &pose_cam, bool usePoseGuess = false, const cv::Vec4f &tablePlane = cv::Vec4f::all(0.0f), cv::Mat *finalJacobian = 0);
private:
  static void computeDistanceTransform(const cv::Mat &edges, int distanceType, int distanceMask, cv::Mat &distanceImage, cv::Mat &dx, cv::Mat &dy);
  static float estimateOutlierError(const cv::Mat &distanceImage, int distanceType);

  //rotate_cam model points
  void setInitialPose(const PoseRT &pose_cam);

  //cache Rt_obj2cam and Rt_cam2obj
  void setObjectCoordinateSystem(const cv::Mat &Rt_obj2cam);
  void getObjectCoordinateSystem(cv::Mat &Rt_obj2cam) const;

  void computeJacobian(const cv::Mat &projectedPoints, const cv::Mat &JaW, const cv::Mat &distanceImage, const cv::Mat &dx, const cv::Mat &dy, cv::Mat &J);
  void computeObjectJacobian(const cv::Mat &projectedPoints, const cv::Mat &inliersMask, const cv::Mat &JaW, const cv::Mat &distanceImage, const cv::Mat &dx, const cv::Mat &dy, const cv::Mat &R_obj2cam, const cv::Mat &t_obj2cam, const cv::Mat &rvec_obj, const cv::Mat &tvec_obj, cv::Mat &J);
  void computeWeightsObjectJacobian(const std::vector<cv::Point3f> &points, const cv::Mat &silhouetteEdges, const PoseRT &pose_obj, cv::Mat &weightsJacobian) const;
  void computeResiduals(const cv::Mat &projectedPoints, cv::Mat &residuals, double outlierError, const cv::Mat &distanceTransform = cv::Mat(), const bool useInterpolation = true) const;
  void computeResidualsWithInliersMask(const cv::Mat &projectedPoints, cv::Mat &residuals, double outlierError, const cv::Mat &distanceTransform, const bool useInterpolation, float inliersRatio, cv::Mat &inliersMask) const;
  double getError(const cv::Mat &residuals) const;

  void computeWeights(const std::vector<cv::Point2f> &projectedPointsVector, const cv::Mat &silhouetteEdges, cv::Mat &weights) const;

  bool isOutlier(cv::Point2f pt) const;
  double getFilteredDistance(cv::Point2f pt, bool useInterpolation, double outlierError = 0., const cv::Mat &distanceTransform = cv::Mat()) const;

  void displayProjection(const cv::Mat &projectedPoints, const std::string &title ) const;

  void projectPoints_obj(const cv::Mat &points, const cv::Mat &rvec_obj, const cv::Mat &tvec_obj, cv::Mat &rvec_cam, cv::Mat &tvec_cam, cv::Mat &Rt_cam, std::vector<cv::Point2f> &projectedPoints, cv::Mat *dpdrot = 0, cv::Mat *dpdt = 0) const;
  void object2cameraTransformation(const cv::Mat &rvec_obj, const cv::Mat &tvec_obj, cv::Mat &Rt_cam) const;

  void computeLMIterationData(int paramsCount, bool isSilhouette,
       const cv::Mat R_obj2cam, const cv::Mat &t_obj2cam, bool computeJacobian,
       const cv::Mat &newTranslationBasis2old, const cv::Mat &rvecParams, const cv::Mat &tvecParams,
       cv::Mat &rvecParams_cam, cv::Mat &tvecParams_cam, cv::Mat &RtParams_cam,
       cv::Mat &J, cv::Mat &error);

  EdgeModel originalEdgeModel;
  EdgeModel rotatedEdgeModel;

  cv::Mat cameraMatrix, distCoeffs;
  cv::Mat extrinsicsRt;

  cv::Mat edgesImage;
  cv::Mat dtImage;
  cv::Mat dtDx, dtDy;

  cv::Mat silhouetteEdges;
  cv::Mat silhouetteDtImage;
  cv::Mat silhouetteDtDx, silhouetteDtDy;

  cv::Mat Rt_obj2cam_cached, Rt_cam2obj_cached;

  cv::Mat cameraMatrix64F;

  LocalPoseRefinerParams params;
  int dim;
  bool hasRotationSymmetry;
  int verticalDirectionIndex;

  friend class PoseQualityEstimator;
};

#endif /* LOCALPOSEREFINER_HPP_ */
