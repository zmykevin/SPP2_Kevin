
#define PY_ARRAY_UNIQUE_SYMBOL pbcvt_ARRAY_API
#include <boost/python.hpp>
#include <pyboostcvconverter/pyboostcvconverter.hpp>


namespace pbcvt {

using namespace boost::python;
/**
 * Example function. Basic inner matrix product using explicit matrix conversion.
 * @param left left-hand matrix operand (NdArray required)
 * @param right right-hand matrix operand (NdArray required)
 * @return an NdArray representing the dot-product of the left and right operands
 */
PyObject* dot(PyObject *left, PyObject *right) {

	cv::Mat leftMat, rightMat;
	leftMat = pbcvt::fromNDArrayToMat(left);
	rightMat = pbcvt::fromNDArrayToMat(right);
	auto c1 = leftMat.cols, r2 = rightMat.rows;
	// Check that the 2-D matrices can be legally multiplied.
	if (c1 != r2){
		PyErr_SetString(PyExc_TypeError,
				"Incompatible sizes for matrix multiplication.");
		throw_error_already_set();
	}
	cv::Mat result = leftMat * rightMat;
	PyObject* ret = pbcvt::fromMatToNDArray(result);
	return ret;
}

//This example uses Mat directly, but we won't need to worry about the conversion
/**
 * Example function. Basic inner matrix product using implicit matrix conversion.
 * @param leftMat left-hand matrix operand
 * @param rightMat right-hand matrix operand
 * @return an NdArray representing the dot-product of the left and right operands
 */
cv::Mat dot2(cv::Mat leftMat, cv::Mat rightMat) {
	auto c1 = leftMat.cols, r2 = rightMat.rows;
	if (c1 != r2) {
		PyErr_SetString(PyExc_TypeError,
				"Incompatible sizes for matrix multiplication.");
		throw_error_already_set();
	}
	cv::Mat result = leftMat * rightMat;

	return result;
}
//Created by Mingyang, to test if this module could work. 
PyObject* minus_two(PyObject *original) {

	cv::Mat originalmat;
	originalmat = pbcvt::fromNDArrayToMat(original);
	cv::Mat result = originalmat -2;
	PyObject* ret = pbcvt::fromMatToNDArray(result);
	return ret;
}
//Created by Mingyang to implement compare to scalar
cv::Mat compare_to_scalar(cv::Mat matrix_1, int scalar)
{
	//First of all, convert it to a specific type
	matrix_1.convertTo(matrix_1,CV_64F);
	//Construct a scalar matrix
	cv::Mat scalar_matrix = Mat::ones(matrix_1.size[0],matrix_1.size[1],CV_64F)*double(scalar);
	//Now Compare the two matrix
	cv::Mat compare_result;
	compare(matrix_1,scalar_matrix,compare_result,CMP_EQ);
	compare_result = compare_result/double(255);
	compare_result.convertTo(compare_result,CV_64F);
	return compare_result;
}

void Sort_Gaussian(cv::Mat* Gaussian_mean, cv::Mat* Gaussian_std, cv::Mat* Gaussian_weight)
{
	Mat result;
	int num_gaussian = Gaussian_mean->size[0];
	Mat W_by_SD = Mat::zeros(num_gaussian,Gaussian_mean->size[1],CV_64F);
	for (int i = 0; i < num_gaussian; i ++)
	{
		Mat division_result;
		divide((*Gaussian_weight).row(i),(*Gaussian_std).row(i),division_result);
		division_result.copyTo(W_by_SD.row(i));
	}
	//Sort W_by_SD and return index
	Mat sort_index;
	sortIdx(W_by_SD,sort_index,SORT_EVERY_COLUMN+SORT_DESCENDING);
	//The next step is to reorder all the parameters with respect to this rank
	Mat original_gaussian_mean = (*Gaussian_mean).clone();
	Mat original_gaussian_std = (*Gaussian_std).clone();
	Mat original_gaussian_weight = (*Gaussian_weight).clone();
	for (int i = 0; i < num_gaussian; i ++)
	{
		Mat new_gaussian_mean_i = Mat::zeros(1,Gaussian_mean->size[1],CV_64F);
		Mat new_gaussian_std_i = Mat::zeros(1,Gaussian_std->size[1],CV_64F);
		Mat new_gaussian_weight_i = Mat::zeros(1,Gaussian_weight->size[1],CV_64F);
		for (int j = 0; j < num_gaussian; j ++)
		{
			Mat comparison_result = compare_to_scalar(sort_index.row(i),j);
			new_gaussian_mean_i += (original_gaussian_mean.row(j)).mul(comparison_result);
			new_gaussian_weight_i += (original_gaussian_weight.row(j)).mul(comparison_result);
			new_gaussian_std_i += (original_gaussian_std.row(j)).mul(comparison_result);
		}
		new_gaussian_mean_i.copyTo(Gaussian_mean->row(i));
		new_gaussian_weight_i.copyTo(Gaussian_weight->row(i));
		new_gaussian_std_i.copyTo(Gaussian_std->row(i));
	}
}

cv::Mat backgroundGaussianProcess(cv::Mat Gaussian_weight, cv::Mat T)
{
	cv::Mat result = Mat::zeros(1,Gaussian_weight.size[1],CV_64F);
	int num_gaussian = Gaussian_weight.size[0];
	for (int i = 0; i < num_gaussian; i ++)
	{
		cv::Mat sum_i;
		cv::reduce(Gaussian_weight.rowRange(0,i+1),sum_i,0,CV_REDUCE_SUM, CV_64F);
		cv::Mat compare_result;
		cv::compare(sum_i,T,compare_result,CMP_GE);
		compare_result = compare_result/255;
		compare_result.convertTo(compare_result,CV_64F);
		result += compare_result; 
	}
	result = num_gaussian-result+1;
	return result;
}

cv::Mat Matching_Gaussian(cv::Mat Gaussian_mean, cv::Mat Gaussian_std, cv::Mat Current_Frame, cv::Mat* history, cv::Mat* Matching_Matrix)
{
	cv::Mat result = Mat::zeros(1,Gaussian_mean.size[1],CV_64F);
	double std_scale = 2.5;
	int num_gaussian = Gaussian_mean.size[0];
	for(int i = 0; i <num_gaussian; i ++)
	{
		cv::Mat difference = cv::abs(Gaussian_mean.row(i)-Current_Frame);
		cv::Mat scaled_std = Gaussian_std.row(i)*std_scale;
		//cout << difference.colRange(329,331) << endl;
		//cout << scaled_std.colRange(329,331) << endl;

		cv::Mat compare_result;
		cv::compare(difference,scaled_std,compare_result,CMP_LE);
		compare_result = compare_result/255;
		compare_result.convertTo(compare_result,CV_64F);
		//cout << compare_result.colRange(329,331) << endl;
		compare_result.copyTo(Matching_Matrix->row(i));//Added by Mingyang
		cv::Mat result_i;

		cv::compare(compare_result-*history,Mat::zeros(1,Gaussian_mean.size[1],CV_64F),result_i,CMP_GT);
		result_i = result_i/255;
		//cout << history->colRange(329,331) << endl;
		//cout << result_i.colRange(329,331) << endl;

		result_i.convertTo(result_i,CV_64F);
		//result_i.copyTo(Matching_Matrix->row(i));//updated Matching_Matrix
		result += (i+1)*result_i;
		
		cv::Mat new_history;
		cv::bitwise_or(compare_result,*history,new_history);
		new_history.convertTo(*history,CV_64F);
	}
	cv::Mat zero_matrix = Mat::zeros(1,Gaussian_mean.size[1],CV_64F);
	cv::Mat modify_zero;
	cv::bitwise_or(*history,zero_matrix,modify_zero);
	modify_zero = (1-modify_zero)*10;
	modify_zero.convertTo(modify_zero,CV_64F);
	result = result + modify_zero;

	return result;
}
/*
void Update_Gaussian(Mat* Gaussian_mean, Mat* Gaussian_std, Mat* Gaussian_weight, Mat Current_Frame, Mat matching_result, Mat matching_matrix,double alpha)
{
	double low_weight = 0.005;
	//Included by Mingyang Zhou
	double initial_std = 6;
	//First let's update the non-matching model
	int num_gaussian = Gaussian_mean->size[0];
	cv::Mat non_matching_model = (double)1-matching_result;
	//cout << non_matching_model.colRange(102,104) << endl;
	cv::Mat original_gaussian_mean = Gaussian_mean->clone();
	cv::Mat original_gaussian_std = Gaussian_std->clone();
	cv::Mat original_gaussian_weight = Gaussian_weight->clone();

	//Update Mean
	//cv::Mat non_matching_new_mean = Current_Frame.mul(non_matching_model)+(original_gaussian_mean.row(num_gaussian-1)).mul(matching_result);
	//non_matching_new_mean.copyTo(Gaussian_mean->row(num_gaussian-1));

	//update Std
	//cv::Mat A = cv::abs(255-Gaussian_mean->row(num_gaussian-1));
	//cv::Mat B = cv::abs(0-Gaussian_mean->row(num_gaussian-1));

	cv::Mat updated_std;
	//cv::max(A,B,updated_std);

	//updated_std = updated_std.mul(non_matching_model)+(original_gaussian_std.row(num_gaussian-1)).mul(matching_result);
	updated_std = initial_std*non_matching_model+(original_gaussian_std.row(num_gaussian-1)).mul(matching_result);
	updated_std.copyTo(Gaussian_std->row(num_gaussian-1));
	//update weight
	//cv::Mat updated_weight = (Gaussian_weight->row(num_gaussian-1)).mul(matching_result);
	cv::Mat updated_weight = (original_gaussian_weight.row(num_gaussian-1)).mul(matching_result) + low_weight*non_matching_model;
	updated_weight.copyTo(Gaussian_weight->row(num_gaussian-1));
	//cv::Mat weight_distribution = (original_gaussian_weight.row(num_gaussian-1)).mul(non_matching_model)/(double)(num_gaussian-1);
	cv::Mat weight_distribution = (abs(original_gaussian_weight.row(num_gaussian-1)-low_weight)).mul(non_matching_model)/(double)(num_gaussian-1);
	for (int i = 0; i < num_gaussian-1; i ++)
	{
		cv::Mat new_weight_i = original_gaussian_weight.row(i) + weight_distribution;
		new_weight_i.copyTo(Gaussian_weight->row(i)); 
	}
	//cout << Gaussian_weight->col(102) << endl;
	original_gaussian_mean = Gaussian_mean->clone();
	original_gaussian_std = Gaussian_std->clone();
	original_gaussian_weight = Gaussian_weight->clone();
	//Update the Matching Model
	for (int i = 0; i < num_gaussian; i ++)
	{
		cv::Mat gaussian_mean_i = original_gaussian_mean.row(i);
		cv::Mat gaussian_weight_i = original_gaussian_weight.row(i);
		cv::Mat gaussian_std_i = original_gaussian_std.row(i)+pow(10.0,-8);

		cv::Mat matching_matrix_i = matching_matrix.row(i);

		//Define the gaussian distribution probability
		cv::Mat difference_square_i;
		cv::pow(cv::abs(Current_Frame-gaussian_mean_i),2,difference_square_i);
		//define the square of standard deviation as varaince
		cv::Mat gaussian_variance_i;
		cv::pow(gaussian_std_i,2,gaussian_variance_i);

		//compute the result inside the exponential function
		cv::Mat inside_exponential = (-0.5)*(difference_square_i/gaussian_variance_i);

		//compute the front part of the gaussian_model
		cv::Mat head_part = (1/sqrt(2*PI))/gaussian_std_i;

		//compute the exponential compoenent
		cv::Mat exponential_component;
		exp(inside_exponential,exponential_component);

		//compute the gaussian desnity distribution
		cv::Mat GDP = head_part.mul(exponential_component);
		cv::Mat P = alpha*GDP;
		
		//update_weight for_the gaussian_model
		cv::Mat matching_k_model_weight = (matching_result).mul(gaussian_weight_i);
		//Updated by Mingyang Zhou
		cv::Mat updated_matching_weight = (non_matching_model).mul(gaussian_weight_i)+(1-alpha)*matching_k_model_weight+alpha*(matching_k_model_weight.mul(matching_matrix_i));
		updated_matching_weight.copyTo(Gaussian_weight->row(i));

		//update mean for matching k models
		
		cv::Mat updated_matching_mean = (1-matching_matrix_i).mul(gaussian_mean_i)+(1-P).mul(matching_matrix_i.mul(gaussian_mean_i))+P.mul(matching_matrix_i.mul(Current_Frame));
		updated_matching_mean.copyTo(Gaussian_mean->row(i));
		

		//update std for matching k models
		 cv::Mat new_varaiance_i;
		 cv::pow((Current_Frame-Gaussian_mean->row(i)),2,new_varaiance_i);
		 cv::Mat updated_matching_variance = (1-P).mul(matching_matrix_i.mul(gaussian_variance_i))+P.mul(matching_matrix_i.mul(new_varaiance_i));
		 cv::Mat updated_std;
		 cv::sqrt(updated_matching_variance,updated_std);
		 updated_std = (1-matching_matrix_i).mul(gaussian_std_i) + updated_std;
		 updated_std.copyTo(Gaussian_std->row(i));
	}
}
*/
void Update_Gaussian(cv::Mat* Gaussian_mean, cv::Mat* Gaussian_std, cv::Mat* Gaussian_weight, cv::Mat Current_Frame, cv::Mat matching_result, cv::Mat matching_matrix,double alpha)
{
	double low_weight = 0.005;
	double initial_std = 6;
	//First let's update the non-matching model
	int num_gaussian = Gaussian_mean->size[0];
	cv::Mat non_matching_model = (double)1-matching_result;
	//cout << non_matching_model.colRange(102,104) << endl;
	cv::Mat original_gaussian_mean = Gaussian_mean->clone();
	cv::Mat original_gaussian_std = Gaussian_std->clone();
	cv::Mat original_gaussian_weight = Gaussian_weight->clone();
    
	//Update Mean
	cv::Mat non_matching_new_mean = Current_Frame.mul(non_matching_model)+(original_gaussian_mean.row(num_gaussian-1)).mul(matching_result);
	//cout << Current_Frame.colRange(102,104) << endl;
	//cout << non_matching_new_mean.colRange(102,104) << endl;
	non_matching_new_mean.copyTo(Gaussian_mean->row(num_gaussian-1));
	//cout << (Gaussian_mean->row(num_gaussian-1)).colRange(102,104) << endl;

	//update Std
	//Mat A = abs(255-Gaussian_mean->row(num_gaussian-1));
	//Mat B = abs(0-Gaussian_mean->row(num_gaussian-1));
	//cout << A.colRange(102,104) << endl;
	//cout << B.colRange(102,104) << endl;

	cv::Mat updated_std;
	//max(A,B,updated_std);
	//cout << updated_std.colRange(102,104) << endl;
	updated_std = initial_std*non_matching_model+(original_gaussian_std.row(num_gaussian-1)).mul(matching_result);
	//updated_std = updated_std.mul(non_matching_model)+(original_gaussian_std.row(num_gaussian-1)).mul(matching_result);
	//cout << updated_std.colRange(102,104) << endl;
	updated_std.copyTo(Gaussian_std->row(num_gaussian-1));
	//update weight
	cv::Mat updated_weight = (original_gaussian_weight.row(num_gaussian-1)).mul(matching_result) + low_weight*non_matching_model;
	//Mat updated_weight = (original_gaussian_weight.row(num_gaussian-1)).mul(matching_result) + (1-alpha)*(original_gaussian_weight.row(num_gaussian-1)).mul(non_matching_model);
	updated_weight.copyTo(Gaussian_weight->row(num_gaussian-1));
	//Discard this weight distribution at this moment.
	cv::Mat weight_distribution = (abs(original_gaussian_weight.row(num_gaussian-1)-low_weight)).mul(non_matching_model)/(double)(num_gaussian-1);
	
	for (int i = 0; i < num_gaussian-1; i ++)
	{
		cv::Mat new_weight_i = original_gaussian_weight.row(i) + weight_distribution;
		new_weight_i.copyTo(Gaussian_weight->row(i)); 
	}
	
	//cout << Gaussian_weight->col(102) << endl;
	original_gaussian_mean = Gaussian_mean->clone();
	original_gaussian_std = Gaussian_std->clone();
	original_gaussian_weight = Gaussian_weight->clone();
	//Update the Matching Model
	for (int i = 0; i < num_gaussian; i ++)
	{
		cv::Mat gaussian_mean_i = original_gaussian_mean.row(i);
		cv::Mat gaussian_weight_i = original_gaussian_weight.row(i);
		cv::Mat gaussian_std_i = original_gaussian_std.row(i)+pow(10.0,-8);

		cv::Mat matching_matrix_i = matching_matrix.row(i);

		//Define the gaussian distribution probability
		cv::Mat difference_square_i;
		pow(abs(Current_Frame-gaussian_mean_i),2,difference_square_i);
		//define the square of standard deviation as varaince
		cv::Mat gaussian_variance_i;
		pow(gaussian_std_i,2,gaussian_variance_i);

		//compute the result inside the exponential function
		cv::Mat inside_exponential = (-0.5)*(difference_square_i/gaussian_variance_i);

		//compute the front part of the gaussian_model
		cv::Mat head_part = (1/sqrt(2*PI))/gaussian_std_i;

		//compute the exponential compoenent
		cv::Mat exponential_component;
		exp(inside_exponential,exponential_component);

		//compute the gaussian desnity distribution
		cv::Mat GDP = head_part.mul(exponential_component);
		cv::Mat P = alpha*GDP;
		
		//update_weight for_the gaussian_model
		cv::Mat matching_k_model_weight = (matching_result).mul(gaussian_weight_i);
		//Updated by Mingyang Zhou
		cv::Mat updated_matching_weight = (non_matching_model).mul(gaussian_weight_i)+(1-alpha)*matching_k_model_weight+alpha*(matching_k_model_weight.mul(matching_matrix_i));
		updated_matching_weight.copyTo(Gaussian_weight->row(i));

		//update mean for matching k models
		
		cv::Mat updated_matching_mean = (1-matching_matrix_i).mul(gaussian_mean_i)+(1-P).mul(matching_matrix_i.mul(gaussian_mean_i))+P.mul(matching_matrix_i.mul(Current_Frame));
		updated_matching_mean.copyTo(Gaussian_mean->row(i));
		

		//update std for matching k models
		 cv::Mat new_varaiance_i;
		 pow((Current_Frame-Gaussian_mean->row(i)),2,new_varaiance_i);
		 cv::Mat updated_matching_variance = (1-P).mul(matching_matrix_i.mul(gaussian_variance_i))+P.mul(matching_matrix_i.mul(new_varaiance_i));
		 cv::Mat updated_std;
		 sqrt(updated_matching_variance,updated_std);
		 updated_std = (1-matching_matrix_i).mul(gaussian_std_i) + updated_std;
		 updated_std.copyTo(Gaussian_std->row(i));
	}
}

PyObject* MOG_Background_Process(PyObject* Current_frame_Py, PyObject* Gaussian_mean_Py, PyObject* Gaussian_std_Py, PyObject* Gaussian_weight_Py)
{
	cv::Mat gaussian_mean,gaussian_std,gaussian_weight,gray_frame;
	(gaussian_mean) = pbcvt::fromNDArrayToMat(Gaussian_mean_Py);
	(gaussian_std) = pbcvt::fromNDArrayToMat(Gaussian_std_Py);
	(gaussian_weight) = pbcvt::fromNDArrayToMat(Gaussian_weight_Py);
	(gray_frame) = pbcvt::fromNDArrayToMat(Current_frame_Py);
	//Define a couple of important parameters
	int frame_height = gray_frame.size[0];
	int frame_width = gray_frame.size[1];
	int K = gaussian_mean.size[0];
	double alpha = 0.005;

	//Initialialize the cv::mat file
	cv::Mat gray_frame_1d = gray_frame.reshape(0,1);
   	gray_frame_1d.convertTo(gray_frame_1d,CV_64F);
    
    //Create a Threshold
    Sort_Gaussian(&gaussian_mean,&gaussian_std,&gaussian_weight);
    
    cv::Mat T = 0.9*Mat::ones(1,frame_height*frame_width,CV_64F);
    //cv::Mat T = gaussian_weight.row(0)+gaussian_weight.row(1);
    //Generate the Background Process
    cv::Mat background_gaussian_model = backgroundGaussianProcess(gaussian_weight,T);

    //Find the Matching Gaussian
    cv::Mat matching_result = Mat::zeros(1,frame_height*frame_width,CV_64F);
    cv::Mat matching_matrix = Mat::zeros(K,frame_height*frame_width,CV_64F);
    cv::Mat matching_model = Matching_Gaussian(gaussian_mean,gaussian_std,gray_frame_1d,&matching_result,&matching_matrix);
    //Print out the result
    cv::Mat subtraction_result_1d;
    cv::compare(matching_model,background_gaussian_model,subtraction_result_1d,CMP_GT);
    
    cv::Mat subtraction_result = subtraction_result_1d.reshape(0,frame_height);
    
    //Update Parameters
    Update_Gaussian(&gaussian_mean,&gaussian_std,&gaussian_weight,gray_frame_1d,matching_result,matching_matrix,alpha);

    PyObject* subtraction_result_py = pbcvt::fromMatToNDArray(subtraction_result);
    return subtraction_result_py;
}

static void init_ar(){
	Py_Initialize();

	import_array();
}

BOOST_PYTHON_MODULE(pbcvt){
	//using namespace XM;
	init_ar();

	//initialize converters
	to_python_converter<cv::Mat,
	pbcvt::matToNDArrayBoostConverter>();
	pbcvt::matFromNDArrayBoostConverter();

	//expose module-level functions
	def("dot", dot);
	def("dot2", dot2);
	def("minus_two",minus_two);//Added by Mingyang
	def("MOG_Background_Process",MOG_Background_Process);//Added by Mingyang
}

} //end namespace pbcvt
