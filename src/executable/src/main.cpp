#include <SFML/Graphics.hpp>
#include <eigen3/Eigen/Core>
#include <homography/homography.hpp>
#include <iostream>
#include <randomGenerator.hpp>
#include <settings.hpp>
#include <timer/timer.hpp>


class ExampleClass : public util::Settings {
 public:
  ExampleClass(const std::string& source_file_name)
      : Settings(source_file_name) {
    // Introduce all membervariables which shall be saved
    // or loaded from the given file.
    // This should be done in the constructor.
    // All the membervariables should have a default
    // value, otherwize whatever value they point to
    // will be safed in the file.
    put<bool>(exampleBool, BOOL_STRNG_ID);
    put<int>(exampleInt, INT_STRNG_ID);
    put<float>(exampleFloat, F_STRING_ID);
    put<double>(exampleDouble, S_STRING_ID);
    put<double, NUM_D_IN_ARRAY>(example_array[0], ARRAY_ID);
  }

  ~ExampleClass() {}

  void print() {
    std::cout << "------<ExampleClass>------\n"
              << BOOL_STRNG_ID << +": " << exampleBool << "\n"
              << INT_STRNG_ID << +": " << exampleInt << "\n"
              << F_STRING_ID << +": " << exampleFloat << "\n"
              << S_STRING_ID << +": " << exampleDouble << "\n"
              << ARRAY_ID << +": "
              << "\n";
    for (size_t i = 0; i < NUM_D_IN_ARRAY; i++) {
      std::cout << "\t[" << i << "] " << std::to_string(example_array[i]) << "\n";
    }
    std::cout << "------</ExampleClass>------" << std::endl;
  }

 private:
  bool exampleBool = true;
  int exampleInt = 42;
  float exampleFloat = 3.141592653589793f;
  double exampleDouble = 3.141592653589793;

  static constexpr int NUM_D_IN_ARRAY = 3;
  std::array<double, NUM_D_IN_ARRAY> example_array;

  // These must be unique
  // These names will be the identifiers in the xml.
  const std::string BOOL_STRNG_ID = "bool";
  const std::string INT_STRNG_ID = "integer";
  const std::string F_STRING_ID = "nearly_pi";
  const std::string S_STRING_ID = "even_more_nearly_pi";
  const std::string ARRAY_ID = "You_should_probably_choose_a_short_name";
};

bool testSFML() {
  std::cout << "testing SFML please press Enter to continue. Then Close Window "
               "to finish."
            << std::endl;
  sf::Window App(sf::VideoMode(800, 600), "myproject");

  while (App.isOpen()) {
    sf::Event Event;
    while (App.pollEvent(Event)) {
      if (Event.type == sf::Event::Closed)
        App.close();
    }
    App.display();
  }

  return true;
}

bool testSettings() {

  const std::string FILE = "ExampleClass.xml";

  ExampleClass exampleClass(FILE);
  std::cout << "These are the values of the registered Members\n"
            << "of exampleClass after constructor was called:" << std::endl;
  exampleClass.print();
  // save the corrent values of all registered membervariables:
  exampleClass.save();
  std::cout << "Now you could look at " << FILE
            << " and change some values. Press Enter when finnished." << std::endl;
  std::getchar();
  // change something in the file and reload into class
  exampleClass.reloadAllFromFile();
  exampleClass.print();

  std::cout << "As you can see the values have changed!\n"
            << "If the file already exists at construction,\n"
            << "the membervariables will be overwritten.\n"
            << "If you run the programm again, you will see\n"
            << "that previous changed values will be loaded\n"
            << "instead of the default values." << std::endl;
  return true;
}

bool testHomography() {

  const Eigen::Vector2d image_size(300, 300);
  std::array<Eigen::Vector2d, 4> A;
  std::array<Eigen::Vector2d, 4> B;
  std::array<EigenSTL::pair_Vector2d_Vector2d, 4> associated_points;
  Eigen::Matrix3d H;

  // Some points in world...
  A[0] << -2, 2;   // A
  A[1] << 2, 2;    // B
  A[2] << 2, -2;   // C
  A[3] << -2, -2;  // D

  // ...should match these picture coordinates
  B[0] << 0, 0;               // A
  B[1] << image_size.x(), 0;  // B
  B[2] << image_size;         // C
  B[3] << 0, image_size.y();  // D

  for (unsigned int i = 0; i < 4; i++) {
    associated_points[i] = std::make_pair(A[i], B[i]);
  }

  // get the transformation matrix
  H = tool::findHomography<tool::SystemSolverMethode::PARTIAL_PIV_LU>(associated_points);

  std::cout << "The homography matrix for the sample points is:\n"
            << H << std::endl;
  // Transform a picture point to the corresponding world point.
  const Eigen::Vector2d a_picture_point(34, 57);
  const Eigen::Vector2d a_world_point =
      tool::computeTransformation(H.inverse(), a_picture_point);

  std::cout << "The Point in A (" << a_picture_point.transpose() << ") corresponds to the point ("
            << a_world_point.transpose() << ") in B." << std::endl;

  // Transform a world point to the corresponding picture point.
  const Eigen::Vector2d b_world_point(2.224, 3.335);
  const Eigen::Vector2d b_picture_point = tool::computeTransformation(H, b_world_point);
  std::cout << "The Point in A (" << b_picture_point.transpose() << ") corresponds to the point ("
            << b_world_point.transpose() << ") in B." << std::endl;

  return true;
}

bool testRandGen() {
  tool::RandomGenerator* rg;
  rg = &rg->getInstance();

  // *********** distributions ***********

  std::cout << " some random numbers:\n";
  std::cout << rg->normalDistribution(0., 1.) << std::endl;
  std::cout << rg->uniformDistribution(0, 1) << std::endl;
  std::cout << rg->zeroOne() << std::endl;
  std::cout << rg->trueFalse() << std::endl;

  constexpr int NUM_VALS = 10;
  constexpr int NUM_EXPERIMENTS = 10000;

  std::cout << "\na Uniform [0,9] distribution:\n";

  std::array<int, NUM_VALS> num_count;
  for (std::size_t i = 0; i < NUM_VALS; i++) {
    num_count[i] = 0;
  }

  tool::UniformDistribution<int> ud(0, NUM_VALS - 1);
  for (int i = 0; i < NUM_EXPERIMENTS; i++) {
    size_t random_number = static_cast<size_t>(ud.get());
    num_count[random_number]++;
  }

  for (int& v : num_count) {
    const int num_ticks = v / static_cast<int>(0.01 * NUM_EXPERIMENTS);
    std::cout << "|";
    for (int j = 0; j < num_ticks; j++) {
      std::cout << "#";
    }
    std::cout << v << std::endl;
    v = 0;
  }

  std::cout << "\na N(4,1.2) distribution:\n";
  tool::NormalDistribution nd(4., 1.2);
  for (int i = 0; i < NUM_EXPERIMENTS; i++) {
    int random_number = std::round(nd.get());
    if (random_number > -1 && random_number < 10) {
      num_count[random_number]++;
    }
  }

  for (int& v : num_count) {
    const int num_ticks = v / static_cast<int>(0.01 * NUM_EXPERIMENTS);
    std::cout << "|";
    for (int j = 0; j < num_ticks; j++) {
      std::cout << "#";
    }
    std::cout << v << std::endl;
  }


  // *********** rand iterators ***********
  std::array<int, 3> a = {{1, 2, 3}};
  std::vector<int> b;
  b.reserve(4);
  for (int i = 0; i < 4; i++) {
    b.emplace_back(ud.get());
  }

  std::cout << "and now a random element of an array:\n[";

  for (const auto& i : a) {
    std::cout << i << ", ";
  }
  std::cout << "]\nchoose [" << *(rg->get<std::array<int, 3>>(a)) << "]" << std::endl;

  std::cout << "and now a random element of an vector:\n[";

  for (const auto& i : b) {
    std::cout << i << ", ";
  }
  std::cout << "]\nchoose [" << *(rg->get<std::vector<int>>(b)) << "]" << std::endl;
}

int main(int argc, char* argv[]) {
  // Usage of timer class
  tool::Timer timer;
  timer.start("test_timer");

  testSFML();
  testSettings();

  testHomography();

  timer.stop("test_timer");

  testRandGen();
}
