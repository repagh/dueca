/* ------------------------------------------------------------------   */
/*      item            : @Module@.cxx
        generated by    : @author@
        from template   : Simulink88Template.cxx (2022.06)
        date            : @date@
        category        : body file
        description     : Encapsulation of the Simulink/rtw model
                          @rtwmodel@
        changes         : @date@ first version
        language        : C++
        copyright       : (c)
*/


#define @Module@_cxx
// include the definition of the module class
#include "@Module@.hxx"

#include <exception>

// include additional files needed for your calculation here
#include <assert.h>

// the standard package for DUSIME, including template source
#define DO_INSTANTIATE
#include <dusime.h>

// include the debug writing header. Warning and error messages
// are on by default, debug and info can be selected by
// uncommenting the respective defines
//#define D_MOD
//#define I_MOD
#include <debug.h>

// you may try to uncomment this define. It might give you slightly
// advanced outputs, but check beforehand with running a test whether
// everything works
//#define ADVANCE_OUTPUT

// class/module name
const char* const @Module@::classname = "@smodule@";

// initial condition/trim table
const IncoTable* @Module@::getMyIncoTable()
{
  static IncoTable inco_table[] = {
    // enter pairs of IncoVariable and VarProbe pointers (i.e.
    // objects made with new), in this table.
    // For example
//    {(new IncoVariable("example", 0.0, 1.0, 0.01))
//     ->forMode(FlightPath, Constraint)
//     ->forMode(Speed, Control),
//     new VarProbe<_ThisModule_,double>
//       (REF_MEMBER(&_ThisModule_::i_example))}

    // always close off with:
    { NULL, NULL} };

  return inco_table;
}

// parameters to be inserted
const ParameterTable* @Module@::getMyParameterTable()
{
  static const ParameterTable parameter_table[] = {
    { "set-timing",
      new MemberCall<_ThisModule_,TimeSpec>
        (&_ThisModule_::setTimeSpec), set_timing_description },

    { "check-timing",
      new MemberCall<_ThisModule_,vector<int> >
      (&_ThisModule_::checkTiming), check_timing_description },

    /* You can extend this table with labels and MemberCall or
       VarProbe pointers to perform calls or insert values into your
       class objects. Please also add a description (c-style string).

       Note that for efficiency, set_timing_description and
       check_timing_description are pointers to pre-defined strings,
       you can simply enter the descriptive strings in the table. */

    /* The table is closed off with NULL pointers for the variable
       label and MemberCall/VarProbe object. The description is used to
       give an overall description of the module. */
    { NULL, NULL, "please give a description of this module"} };

  return parameter_table;
}

// constructor
@Module@::@Module@(Entity* e, const char* part, const
                       PrioritySpec& ps) :
  /* The following line initialises the SimulationModule base class.
     You always pass the pointer to the entity, give the classname and the
     part arguments.
     If you give a NULL pointer instead of the inco table, you will not be
     called for trim condition calculations, which is normal if you for
     example implement logging or a display.
     If you give 0 for the snapshot state, you will not be called to
     fill a snapshot, or to restore your state from a snapshot. Only
     applicable if you have no state. */
  SimulationModule(e, classname, part, getMyIncoTable(), NSTATES),

  // initialize the data you need in your simulation
  S(NULL),
  Snew(NULL),
  Sold(NULL),
  dt(-1.0),
  i_S(NULL),

  // initialize the data you need in your simulation or process

  // initialize the channel access tokens, check the documentation for the
  // various parameters. Some examples:
  // r_mytoken(getId(), NameSet(getEntity(), getclassname<MyData>(), part),
  //           getclassname<MyData>(), 0, Channel::Events),
  // w_mytoken(getId(), NameSet(getEntity(), getclassname<MyData2>(), part),
  //           getclassname<MyData2>(), "label", Channel::Continuous),

  // create a clock, if you need time based triggering
  // instead of triggering on the incoming channels
  // myclock(),

  // activity initialization
  cb1(this, &_ThisModule_::doCalculation),
  do_calc(getId(), "@activityname@", &cb1, ps)
{
  // do the actions you need for the simulation

  // uncomment and complete to connect the triggers for simulation
  // do_calc.setTrigger(/* fill in your triggering channels */);

  // uncomment and complete to connect the triggers for trim
  // calculation. Leave this out if you not need input for trim
  // calculation
  // trimCalculationCondition(/* fill in your trim triggering channels */);

  // pleas add a check that the RTW model sizes are what you are expecting
  // assert(NINPUTS == );
  // assert(NOUTPUTS == );
  // assert(NCSTATES == );

  /* ---------------------------------------------------------------- */
  /* Real-time workshop model initialisation                          */

  // initialise global stuff
  // rt_InitInfAndNaN(sizeof(real_T));

  // create the model
  S = createAndInitialiseModel();

  /* ---------------------------------------------------------------- */
  /* Real-time workshop model initialisation for trim calculator      */
  i_S = createAndInitialiseModel();

  /* some checks. */
  assert(sizeof(real_T) == sizeof(double));
}

class except_@rtwmodel@: public std::exception
{
public:
  /** Re-implementation of std:exception what. */
  const char* what() const throw() {return "coder model @rtwmodel@ error";}
};

@Module@::modelSet *@Module@::createAndInitialiseModel()
{
  modelSet *S = new modelSet();
  S->M = @rtwmodel@(&(S->U), &(S->Y));
  if (S->M == NULL) {
    E_MOD(getId() << " Cannot create model \"@rtwmodel@\"");
    throw(except_@rtwmodel@());
  }
  if (rtmGetErrorStatus(S->M) != NULL) {
    E_MOD(getId() << " Error registering model \"@rtwmodel@\"" <<
          rtmGetErrorStatus(S->M));
    throw(except_@rtwmodel@());
  }

  // further initialisation
  @rtwmodel@_initialize(S->M, &(S->U), &(S->Y));

  return S;
}

void @Module@::destroyModel(modelSet *dS)
{
  @rtwmodel@_terminate(dS->M);
  delete dS;
}

bool @Module@::complete()
{
  /* All your parameters have been set. You may do extended
     initialisation here. Return false if something is wrong. */
  return true;
}

// destructor
@Module@::~@Module@()
{
  destroyModel(S);
  destroyModel(i_S);
}

// as an example, the setTimeSpec function
bool @Module@::setTimeSpec(const TimeSpec& ts)
{
  // a time span of 0 is not acceptable
  if (ts.getValiditySpan() == 0) return false;

  // specify the timespec to the activity
  do_calc.setTimeSpec(ts);

  // do whatever else you need to process this in your model

  // return true if everything is acceptable
  return true;
}

// and the checkTiming function
bool @Module@::checkTiming(const vector<int>& i)
{
  if (i.size() == 3) {
    new TimingCheck(do_calc, i[0], i[1], i[2]);
  }
  else if (i.size() == 2) {
    new TimingCheck(do_calc, i[0], i[1]);
  }
  else {
    return false;
  }
  return true;
}

// tell DUECA you are prepared
bool @Module@::isPrepared()
{
  bool res = true;

  // Example checking a token:
  // CHECK_TOKEN(w_somedata);

  // Example checking anything
  // CHECK_CONDITION(myfile.good());

  // return result of checks
  return res;
}

// start the module
void @Module@::startModule(const TimeSpec &time)
{
  do_calc.switchOn(time);
}

// stop the module
void @Module@::stopModule(const TimeSpec &time)
{
  do_calc.switchOff(time);
}

// fill a snapshot with state data. You may remove this method (and the
// declaration) if you specified to the SimulationModule that the size of
// state snapshots is zero
void @Module@::fillSnapshot(const TimeSpec& ts,
                              Snapshot& snap, bool from_trim)
{
  // The most efficient way of filling a snapshot is with an AmorphStore
  // object.
  AmorphStore s(snap.accessData(), snap.getDataSize());

  if (from_trim) {
    // read the data you want to snap from i_S; note that the MathWorks
    // take pleasure in changing the interface to the model with every
    // minor version; inspect your header file to see how to read the
    // variables
  }
  else {
    // pack the data you snapped to the snapshot
  }
}

// reload from a snapshot. You may remove this method (and the
// declaration) if you specified to the SimulationModule that the size of
// state snapshots is zero
void @Module@::loadSnapshot(const TimeSpec& t, const Snapshot& snap)
{
  if (Sold != NULL) {
    destroyModel(Sold);
    Sold = NULL;
  }

  // check that there is room to make a new model
  if (Snew != NULL) {
    W_MOD(getId() << '/' << classname << " no room for snapshot");
    return;
  }

  // access the data in the snapshot with an AmorphReStore object
  AmorphReStore s(snap.data, snap.getDataSize());

  // Start with a new model. There are simply too much side effects in
  // the model creation functions (setting work vectors to 0, etc.),
  // to believe that a proper reset is possible. We found often enough
  // that a second run with a re-initialised model produces strange
  // effects.
  modelSet *S2 = createAndInitialiseModel();

  // restore the state vector. Note that the re-start puts the state
  // vector to the initial one, we have to do it after, or otherwise
  // this is undone. Make this match the snapping above

  // new model is ready now, leave in pick-up location
  Snew = S2;
}

// this routine contains the main simulation process of your module. You
// should read the input channels here, and calculate and write the
// appropriate output
void @Module@::doCalculation(const TimeSpec& ts)
{
  // check whether there is a new model waiting.
  if (Snew != NULL) {
    Sold = S; S = Snew; Snew = NULL;
  }

  // copy the input to the model
  // example:
  // try {
  //   DataReader<MyInput> u(input_token, ts);
  //   S->U.throttle = u.data().throttle;
  //   S->U.de = u.data().de; ....
  // }
  // catch(Exception& e) {
  //   // strange, there is no input. Should I try to continue or not?
  // }

  // check the state we are supposed to be in
  switch (getAndCheckState(ts)) {
  case SimulationState::HoldCurrent: {

    break;
    }

  case SimulationState::Replay:
  case SimulationState::Advance: {
    /* The above piece of code shows a block in which you try to catch
       error conditions (exceptions) to handle the case in which the input
       data is lost. This is not always necessary, if you normally do not
       foresee such a condition, and you don t mind being stopped when
       it happens, forget about the try/catch blocks. */

    // run an update
    @rtwmodel@_output(S->M, &(S->U), &(S->Y));
    @rtwmodel@_update(S->M, &(S->U), &(S->Y));

    break;
    }
  default:
    // other states should never be entered for a SimulationModule,
    // HardwareModules on the other hand have more states. Throw an
    // exception if we get here,
    throw CannotHandleState(getId(),GlobalId(), "state unhandled");
  }

  // DUECA applications are data-driven. From the time a module is switched
  // on, it should produce data, so that modules "downstreams" are
  // activated

  // access your output channel(s)
  // example
  // DataWriter<MyOutput> y(output_token, ts);

  // write the output into the output channel, using the stream writer
  // y.data().var1 = S->Y.var1; ...

  if (snapshotNow()) {
    // keep a copy of the model state. Snapshot sending is done in the
    // sendSnapshot routine, later, and possibly at lower priority
  }
}

/* WARNING                       WARNING               WARNING */
/* trim calculation stuff has not been verified. */

void @Module@::trimCalculation(const TimeSpec& ts, const TrimMode& mode)
{
  // read the event equivalent of the input data, and feed that to the
  // trim model
  // example:
  //{
    // DataReader<MyInput> u(i_input_token, ts);
    // i_S->U.... = u.data().....
  //}

  // using the input, and the data put into your trim variables,
  // calculate the derivative of the state. DO NOT use the state
  // vector of the normal simulation here, because it might be that
  // this is done while the simulation runs!
  // Some elements in this state derivative are needed as target, copy
  // these out again into trim variables (see you TrimTable

  // trim calculation
  switch(mode) {
  case FlightPath: {
    // one type of trim calculation, find a power setting and attitude
    // belonging to a flight path angle and speed
  }
  break;

  case Speed: {
    // find a flightpath belonging to a speed and power setting (also
    // nice for gliders)
  }
  break;

  case Ground: {
    // find an altitude/attitude belonging to standing still on the
    // ground, power/speed 0
  }
  break;

  default:
    W_MOD(getId() << " cannot calculate inco mode " << mode);
  break;
  }
  @rtwmodel@_update(S->M, &(S->U), &(S->Y));

  // This works just like a normal calculation, only you provide the
  // steady state value (if your system is stable anyhow). So, if you
  // have other modules normally depending on your output, you should
  // also produce the equivalent output here.
  // DataWriter<MyOutput> y(output_token, ts);

  // write the output into the output channel, using the DataWriter

  // now return. The real results from the trim calculation, as you
  // specified them in the TrimTable, will now be collected and sent
  // off for processing.
}

// Make a TypeCreator object for this module, the TypeCreator
// will check in with the scheme-interpreting code, and enable the
// creation of modules of this type
static TypeCreator<@Module@> a(@Module@::getMyParameterTable());

