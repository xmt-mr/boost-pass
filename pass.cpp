// g++ -o pass pass.cpp -std=c++11 -lboost_python37 -lboost_numpy37 `python3-config --cflags` `python3-config --ldflags` `pkg-config --libs opencv`

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <opencv2/core/core.hpp>

#include <vector>

namespace p = boost::python;
namespace np = boost::python::numpy;

using namespace std;

int main() {
    // initialization
    Py_Initialize();
    np::initialize();

    p::object main_ns = p::import("__main__").attr("__dict__");

    try {
        ifstream ifs("pass.py");
        string script((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
        p::exec(script.c_str(), main_ns);
        p::object printX = main_ns["printX"];


        // float -> float
        float X1 = 2;
        cout << "int - original: " << X1 << endl;
        p::object X1_result = printX(X1);
        cout << "c++:\n" << p::extract<float>(X1_result) << endl;



        // array -> ndarray -> array
        int X2[][2] = {{0, 1}, {2, 3}, {4, 5}, {6, 7}};

        // オリジナルを表示
        cout << "\narray - original: " << endl;
        for(int i=0 ; i<4 ; i++){
            for(int j=0; j<2 ; j++){
                cout << X2[i][j] << " ";
            }
            cout << endl;
        }

        // ndarrayに変換
        np::dtype dt2 = np::dtype::get_builtin<int>();
        p::tuple shapeX2 = p::make_tuple(4,2);
        p::tuple strideX2 = p::make_tuple(sizeof(int)*2, sizeof(int));
        p::object ownerX2;
        np::ndarray X2_ndarray = np::from_data(X2, dt2, shapeX2, strideX2, ownerX2);

        // pythonにわたし，ndarrayとして受け取る
        p::object X2_return = printX(X2_ndarray);
        np::ndarray X2_return_ndarray = p::extract<np::ndarray>(X2_return);

        Py_intptr_t const * shapeX2_return = X2_return_ndarray.get_shape();
        //Py_intptr_t const * stridesX2_return = X2_return_ndarray.get_strides();

        // 型
        //cout << p::extract<char const *>(p::str(X2_return_ndarray.get_dtype())) << endl;

        // 配列にコピー
        float X2_result[ shapeX2_return[0] ][ shapeX2_return[1] ];
        for (int i = 0; i < shapeX2_return[0]; ++i) {
            for (int j = 0; j < shapeX2_return[1]; ++j) {
                X2_result[i][j] = p::extract<int>(X2_return_ndarray[i][j]);
            }
        }

        // 結果を表示
        cout << "c++:" << endl;
        for(int i=0 ; i<4 ; i++){
            for(int j=0; j<2 ; j++){
                cout << X2[i][j] << " ";
            }
            cout << endl;
        }



        // 1d vector
        vector<float> X3{4.0, 6.0, 5.0}; // C++11

        // オリジナルを表示
        cout << "\n1d vector - original: " << endl;
        for(unsigned int i=0; i<X3.size() ; i++) cout << X3[i] << " ";
        cout << endl;

        // ndarray に変換
        np::dtype dt3 = np::dtype::get_builtin<float>();
        p::tuple shapeX3 = p::make_tuple(X3.size());
        //p::tuple strideX3 = p::make_tuple(sizeof(float));
        // vectorは余分に領域が確保されることが多く、その場合は以下の方法は用いることができない
        //np::ndarray X3_ndarray = np::from_data(X3.data(), dt3, shapeX3, strideX3, p::object()); // X3.data() returns pointer at beginning

        // こちらの使用を推奨
        np::ndarray X3_ndarray = np::zeros(shapeX3, dt3);
        copy(X3.begin(), X3.end(), reinterpret_cast<float*>(X3_ndarray.get_data()));

        // pythonにわたし，ndarrayとして受け取る
        p::object X3_return = printX(X3_ndarray);
        np::ndarray X3_return_ndarray = p::extract<np::ndarray>(X3_return);

        // vectorを生成
        //Py_intptr_t const * shapeX3_return = X3_return_ndarray.get_shape();
        vector<float> X3_result(reinterpret_cast<float*>(X3_return_ndarray.get_data()), 
            reinterpret_cast<float*>(X3_return_ndarray.get_data()+sizeof(float)*X3_return_ndarray.get_shape()[0]));

        // 結果を表示
        cout << "c++:" << endl;
        for(float x : X3_result) cout << x << " ";
        cout << endl;



        // 2d vector
        vector< vector<float>> X4{{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}}; 

        // オリジナルを表示
        cout << "\n2d vector - original: " << endl;
        for(auto v: X4){ 
            for(auto x: v) cout << x << " ";
            cout << endl;
        }
        cout << endl;

        // ndarrayに変換
        np::dtype dt4 = np::dtype::get_builtin<float>();
        p::tuple shapeX4 = p::make_tuple(X4.size(), X4[0].size());
        //p::tuple strideX4 = p::make_tuple(sizeof(float)*X4[0].size(), sizeof(float));
        np::ndarray X4_ndarray = np::zeros(shapeX4, dt4);
        Py_intptr_t const * strideX4 = X4_ndarray.get_strides();
        for(int i=0 ; i < shapeX4[0] ; i++) copy(X4[i].begin(), X4[i].end(), reinterpret_cast<float*>(X4_ndarray.get_data() + i * strideX4[0]));

        //np::ndarray X4_ndarray = np::from_data(X4[0].data(), dt4, shapeX4, strideX4, p::object());

        // pythonにわたし，ndarrayとして受け取る
        p::object X4_return = printX(X4_ndarray);
        np::ndarray X4_return_ndarray = p::extract<np::ndarray>(X4_return);

        // 2d vectorを生成
        Py_intptr_t const * shapeX4_return = X4_return_ndarray.get_shape();
        Py_intptr_t const * strideX4_return = X4_return_ndarray.get_strides();
        vector< vector<float>> X4_result;
        for (int i = 0; i < shapeX4_return[0]; ++i) {
            X4_result.push_back(vector<float>(reinterpret_cast<float*>(X4_return_ndarray.get_data() + i * strideX4_return[0]), 
                reinterpret_cast<float*>(X4_return_ndarray.get_data() + i * strideX4_return[0] + shapeX4_return[1] * strideX4_return[1])));
        }

        // 結果を表示
        cout << "c++:" << endl;
        for(auto v: X4_result){ 
            for(auto x: v) cout << x << " ";
            cout << endl;
        }
        cout << endl;


        // 2d cv::mat
        cv::Mat X5 = (cv::Mat_<float>(4,2) << 1, 2, 3, 4, 5, 6, 7, 8);

        // オリジナルを表示
        cout << "\n2d matrix - original: " << endl;
        cout << X5 << endl;

        np::dtype dt5 = np::dtype::get_builtin<float>();
        p::tuple shapeX5 = p::make_tuple(X5.rows, X5.cols);
        p::tuple strideX5 = p::make_tuple(sizeof(float)*X5.cols, sizeof(float));

        std::cout << "isContinuous:" << (X5.isContinuous()?"true":"false") << std::endl; // 基本的には連続
        if(X5.isContinuous()){
            // 連続している時は，先頭のポインタを渡せばndarrayが生成できる
            np::ndarray X5_ndarray = np::from_data(X5.ptr(0), dt5, shapeX5, strideX5, p::object());

            // python
            p::object X5_return = printX(X5_ndarray);
            np::ndarray X5_return_ndarray = p::extract<np::ndarray>(X5_return);

            // matに変換
            Py_intptr_t const * shapeX5_return = X5_return_ndarray.get_shape();
            cv::Mat X5_result = cv::Mat_<float>(shapeX5_return[0], shapeX5_return[1], reinterpret_cast<float*>(X5_return_ndarray.get_data()));

            // 結果
            cout << "c++:" << endl;
            cout << X5_result << endl;
        }


        // tuple
        p::tuple t1 = p::make_tuple(3, 2, 1);
        p::tuple t2 = p::extract<p::tuple>(printX(t1));
        for(int i=0 ; i < p::len(t2) ; i++) cout << p::extract<int>(t2[i]) << " ";
        cout << endl;


        // list
        vector<string> X6{"123", "abcd", "45efg"}; // C++11
        p::list p_list;
        for(string str : X6){
            p_list.append(str);
        }
        p::list p_list2 = p::extract<p::list>(printX(p_list));
        for(int i=0 ; i < p::len(p_list2) ; i++){
            string c_str = p::extract<string>(p_list2[i]);
            cout << c_str << " ";
        }
        cout << endl;


    }
    catch (p::error_already_set) {
        PyErr_Print();
    }
}