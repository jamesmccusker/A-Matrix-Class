#include<iostream>
#include<cstdlib> // for c style exit
#include<vector>
#include<cmath>

class matrix
{
    // Friends
    friend std::ostream& operator<<(std::ostream& os, const matrix& mat);
    friend std::istream& operator>>(std::istream& is, const matrix& mat);
private:
    double* matrix_data{ nullptr };
    int rows{ 0 };
    int columns{ 0 };
public:
    // Default constructor
    matrix() = default;

    // Parameterized constructor
    matrix(size_t row, size_t col, std::vector<double> matrix_elements) noexcept
    {
        if (row < 1 || col < 1) {
            throw("size not positive");
        }
        rows = row;
        columns = col;
        matrix_data = new double[rows * columns];
        for (size_t i{ 0 }; i < rows * columns; i++) { matrix_data[i] = matrix_elements[i]; }
    }

    // Copy constructor
    matrix(const matrix& new_matrix) noexcept {
        // Copy size and declare new array
        rows = new_matrix.rows;
        columns = new_matrix.columns;
        matrix_data = new double[rows * columns];

        // Copy values into new array
        for (int i = 0; i < rows * columns; i++) {
            matrix_data[i] = new_matrix.matrix_data[i];
        }
    }

    // Move constructor
    matrix(matrix&& arr) noexcept : rows(arr.rows), columns(arr.columns), matrix_data(arr.matrix_data) { 
        arr.rows = 0;
        arr.columns = 0;
        arr.matrix_data = nullptr;
    }

    // Destructor
    ~matrix() noexcept {
        delete[] matrix_data;
    }

    // Access functions
    size_t get_rows() const { return rows; } // Return number of rows
    size_t get_cols() const { return columns; } // Return number of columns
    size_t index(size_t m, size_t n) const // Return position in array of element (m,n)
    {
        if (m > 0 && m <= rows && n > 0 && n <= columns) return (n - 1) + (m - 1) * columns;
        else { std::cout << "Error: out of range" << std::endl; exit(1); }
    }

    matrix& operator=(const matrix& arr) {
        if (&arr == this) return *this; 
        // First delete this object's array
        delete[] matrix_data; matrix_data = nullptr;
        rows = arr.rows;
        columns = arr.columns;
        // Now copy size and declare new array
        matrix_data = new double[rows * columns];
        // Copy values into new array
        for (int i = 0; i < rows * columns; i++)
        {
            matrix_data[i] = arr.matrix_data[i];
        }
        return *this;
    }

    // Move Assignment operator

    matrix& operator=(matrix&& arr) noexcept
    {
        if (this != &arr) {
            std::swap(rows, arr.rows);
            std::swap(columns, arr.columns);
            std::swap(matrix_data, arr.matrix_data);
            
        }
        return *this;
    }
        

    // Addition operator

    matrix operator+(const matrix& a) {
        if (a.get_rows() != rows || a.get_cols() != columns) {
            throw std::invalid_argument("Matrices must have the same size to be added.");
        }

        std::vector<double> result_data;
        for (int i = 1; i <= rows; i++) {
            for (int j = 1; j <= columns; j++) {
                result_data.push_back(a.matrix_data[a.index(i, j)] + matrix_data[this->index(i, j)]);
            }
        }

        return matrix(a.get_rows(), columns, result_data);
    }

    // Subtraction operator
    matrix operator-(const matrix& a) {
        if (a.get_rows() != rows || a.get_cols() != columns) {
            throw std::invalid_argument("Matrices must have the same size to be subtracted.");
        }

        std::vector<double> result_data;
        for (int i = 1; i <= rows; i++) {
            for (int j = 1; j <= columns; j++) {
                result_data.push_back(matrix_data[this->index(i, j)] - a.matrix_data[a.index(i, j)]);
            }
        }

        return matrix(a.get_rows(), columns, result_data);
    }

    // Multiplication operator
    matrix operator*(const matrix& a) {
        if (rows != a.get_cols()) {
            std::cout << "Error: the first matrix must have the same number of columns as the second matrix has rows." << std::endl;
            return matrix();
        }

        std::vector<double> result_data(a.get_cols() * rows);
        for (int i = 1; i <= rows; i++) {
            for (int j = 1; j <= a.get_cols(); j++) {
                double sum = 0;
                for (int k = 1; k <= columns; k++) {
                    sum += matrix_data[this->index(i, k)] * a.matrix_data[a.index(k, j)];
                }
                result_data[(i - 1) * a.get_cols() + (j - 1)] = sum;
            }
        }

        return matrix(rows, a.get_cols(), result_data);
    }

    // Function to remove a row and column
    matrix delete_row_and_column(int row_to_delete, int column_to_delete) {
        int new_rows = rows - 1;
        int new_cols = columns - 1;
        std::vector<double> new_matrix_data(new_rows * new_cols);

        int new_index = 0;
        for (int i = 1; i <= rows; i++) {
            for (int j = 1; j <= columns; j++) {
                if (i != row_to_delete && j != column_to_delete) {
                    new_matrix_data[new_index] = matrix_data[this->index(i, j)];
                    new_index++;
                }
            }
        }

        return matrix(new_rows, new_cols, new_matrix_data);
    }
    
    // Calculation of the determinant
    double det() {
        if (rows != columns) {
            std::cout << "Error: determinant is only defined for square matrices" << std::endl;
            return 0;
        }
        if (rows == 1) {
            return matrix_data[0];
        }
        double determinant = 0;
        for (int j = 1; j <= columns; j++) {
            matrix minor;
            minor = this->delete_row_and_column(1, j);
            determinant += ((j % 2 == 0) ? -1 : 1) * matrix_data[index(1, j)] * minor.det();
        }
        return determinant;
    }

};

// Overload to input stream for matrices

std::istream& operator>>(std::istream& is, const matrix& mat) {
    if (is.fail()) {
        std::cout << "Invalid input" << std::endl;
    }
    {
        for (int i = 0; i < mat.rows * mat.columns; i++)
        {
            is >> mat.matrix_data[i];
        }
        return is;
    }
}

// Overload insertion to output stream for matrices
std::ostream& operator<<(std::ostream& os, const matrix& mat) {
    for (int i = 1; i <= mat.get_rows(); i++)
    {
        for (int j = 1; j <= mat.get_cols(); j++)
        {
            os << mat.matrix_data[(i - 1) * mat.get_cols() + (j - 1)] << " ";
        }
        os << std::endl;
    }
    return os;
}


// Main program
int main()
{
   
    // Demonstrate default constructor
    matrix a1;
    std::cout << "a1 (default matrix)  \n";
    std::cout << a1 << std::endl;

    // Parameterized constructor

    const int m{ 2 };
    const int n{ 2 };
    std::vector<double> matrix_elements_a2{ 1, 2, 3, 4 };
    matrix a2(m, n, matrix_elements_a2);

    // Print matrix a2

    std::cout << "a2 = \n";
    std::cout << a2 << std::endl;

    // Deep copy by assignment: define new matrix a3 then copy from a2 to a3
    std::cout << "Deep copy by assigment (copy a2 to a3)" << std::endl;
    std::vector<double> matrix_elements_a3{ 1, 1, 1, 1 };
    matrix a3(m, n, matrix_elements_a3);
    std::cout << "a3 = \n";
    std::cout << a3 << std::endl;
    a3 = a2;
    std::cout << "a3 (copied from a2) = \n";
    std::cout << a3 << std::endl;

    // Modify contents of original matrix and show assigned matrix is unchanged here

    std::vector<double> matrix_elements_a2_modified{ 5, 5, 5, 5 };
    a2 = matrix(m, n, matrix_elements_a2_modified);
    
    std::cout << "a2 (modified) = \n";
    std::cout << a2 << std::endl;
    std::cout << "a3 (unchanged) = \n";
    std::cout << a3 << std::endl;

    // Deep copy using copy constructor
    std::cout << "Deep copy using copy constructer (Copy a4 from a2)" << std::endl;
    matrix a4{ a2 };
    std::cout << "a4 (deep copied from a2) = \n";
    std::cout << a4 << std::endl;

    // Modify contents of original matrix and show copied matrix is unchanged here

    std::vector<double> matrix_elements_a2_modified_new{ 2, 4, 6, 8 };
    a2 = matrix(m, n, matrix_elements_a2_modified_new);

    std::cout << "a2 (modified) = \n";
    std::cout << a2 << std::endl;
    std::cout << "a4 (unchanged) = \n";
    std::cout << a4 << std::endl;

    // Move assignment demonstration
    matrix a5;
    std::cout << "Move assignment demonstration (Move a3 to a5)" << std::endl;
    a5 = std::move(a3);
    std::cout << "a3 (empty) = \n";
    std::cout << a3 << std::endl;
    std::cout << "a5 = \n ";
    std::cout << a5 << std::endl;
    
    // Move constructor demonstration
    std::cout << "Move constructer demonstration. (Move a4 to a6)" << std::endl;
    matrix a6{ std::move(a4) };
    std::cout << "a4 (empty) = \n";
    std::cout << a4 << std::endl;
    std::cout << "a6 = \n";
    std::cout << a6 << std::endl;
    
    
    // Demonstration of overload to input stream 
    int num_rows, num_cols;
    std::cout << "Please enter the number of rows for your input matrix: ";
    std::cin >> num_rows;
    std::cout << "Please enter the number of columns: ";
    std::cin >> num_cols;
    std::vector<double> matrix_elements(num_rows * num_cols);
    matrix matrix_1(num_rows, num_cols, matrix_elements);

    std::cout << "Please enter the elements of the matrix, row by row:" << std::endl;
    std::cin >> matrix_1;
    std::cout << "The matrix you entered is: " << matrix_1 << std::endl;

    // Matrix operations

    std::vector<double> matrix_elements_A{ 1, 2, 3, 9, 8, 7, 4, 2, 6 };
    matrix A(3, 3, matrix_elements_A);

    std::vector<double> matrix_elements_B{ 5, 5, 4, 1, 2, 3, 6, 9, 8 };
    matrix B(3, 3, matrix_elements_B);

    std::vector<double> matrix_elements_C{ 3, 4, 1, 2, 5, 6 };
    matrix C(3, 2, matrix_elements_C);

    // Addition of 2 matrices

    std::cout << "(A + B) = \n ";
    std::cout << A + B << std::endl;

    // Subtraction of 2 matrices

    std::cout << "(A - B) = \n ";
    std::cout << A - B << std::endl;

    // Multiplication of 2 matrices

    std::cout << "(A * B) = \n ";
    std::cout << A * B << std::endl;
    std::cout << "(C * B) = \n ";
    std::cout << C * B << std::endl;
    std::cout << "(B * C) = \n ";
    std::cout << B * C << std::endl;

    // Determinant

    std::cout << "Determinant of A = " << A.det() << std::endl;
    std::cout << "Determinant of B = " << B.det() << std::endl;

    return 0;
}