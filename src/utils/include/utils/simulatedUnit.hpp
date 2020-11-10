/**
 * @file SimulatedUnit.h
 * @brief: All things which change over the time within the simulationprogram and thus have to be simulated are a child class of this class.
 * To simplify everything and allow multi tasking: All Units within the simulation time delta t are independent of eache other.
 * @date 10.04.2017
 * @author Jakob Wandel
 * @version 1.0
 **/
#ifndef SIMULATEDUNIT
#define SIMULATEDUNIT
#include <timer/timer.hpp>

class SimulatedUnit {
 protected:
  tool::PreciseTime t0;  //[time] Start time since this unit was created.
  tool::PreciseTime t_last_update;  //[time] Time when this unit was last updated.
  tool::PreciseTime delta_t_update;  //[time] After this intervall this unit needs an update.
  bool has_updated;  // for visualization->if true the unit needs a visual update

 public:
  /*!
   * \brief default constructor
   */
  SimulatedUnit(tool::PreciseTime t0) : t0(t0) {}
  ~SimulatedUnit() {}

  /*!
   * \brief: To run one simulation step from a child class run this function.
   * \param tnow: The time difference since the simulation started.
   */
  void simulate(tool::PreciseTime tnow) {
    if (runSimulationStep(tnow)) {
      update(tnow);
      t_last_update = tnow;
    }
  }

  /*!
   * \brief calculates the age of the simulated unit.
   * \param tnow: The time difference since the simulation started.
   *\return age of simulated unit
   */
  tool::PreciseTime getAge(tool::PreciseTime tnow) { return tnow - this->t0; }

  /*!
   * \brief estimates the age of the simulated unit using its last update.
   *\return age of simulated unit
   */
  tool::PreciseTime getAge() { return getAge(t_last_update); }

  /*!
   * \brief Checks, if this simulated unit was updated since the last time
   * asked. E.g. for redrawing purpose. \return True if the unit was updated.
   */
  bool wasUpdated() {
    if (this->has_updated) {
      this->has_updated = false;
      return true;
    }
    return false;
  }

 protected:
  /*!
   * \brief Since different simulated units need to be updated in different time intervalls this function checks wheather this unit needs a update or not.
   * \param tnow: The time difference since the simulation started.
   * \return bool: Returns true, if this unit needs to be updated.
   */
  bool runSimulationStep(tool::PreciseTime tnow) {
    if (delta_t_update > tnow - t_last_update) {
      return false;
    } else {
      this->has_updated = true;
      return true;
    }
  }

  /*!
   * \brief: Every child class which inherits from this class needs this update
   *function, which will be called when this simulated unit needs to be updated.
   * \param The time difference since the simulation started.
   */
  virtual void update(tool::PreciseTime tnow){};
};
#endif  // !_SIMULATEDUNIT_
