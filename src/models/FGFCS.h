/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 Header:       FGGFCS.h
 Author:       Jon S. Berndt
 Date started: 12/12/98

 ------------- Copyright (C) 1999  Jon S. Berndt (jsb@hal-pc.org) -------------

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA  02111-1307, USA.

 Further information about the GNU General Public License can also be found on
 the world wide web at http://www.gnu.org.

HISTORY
--------------------------------------------------------------------------------
12/12/98   JSB   Created

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
SENTRY
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#ifndef FGFCS_H
#define FGFCS_H

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#ifdef FGFS
#  include <simgear/compiler.h>
#  ifdef SG_HAVE_STD_INCLUDES
#    include <vector>
#  else
#    include <vector.h>
#  endif
#else
#  include <vector>
#endif

#include <string>
#include <models/flight_control/FGFCSComponent.h>
#include <models/FGModel.h>
#include <models/FGLGear.h>
#include <input_output/FGXMLElement.h>

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
DEFINITIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#define ID_FCS "$Id: FGFCS.h,v 1.3 2005/06/13 16:59:17 ehofman Exp $"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
FORWARD DECLARATIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

namespace JSBSim {

typedef enum { iDe=0, iDaL, iDaR, iDr, iDsb, iDsp, iDf, NNorm } FcIdx;
typedef enum { ofRad=0, ofNorm, ofMag , NForms} OutputForm;

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DOCUMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/** Encapsulates the Flight Control System (FCS) functionality.
    This class owns and contains the list of FGFCSComponents
    that define the control system for this aircraft. The config file for the
    aircraft contains a description of the control path that starts at an input
    or command and ends at an effector, e.g. an aerosurface. The FCS components
    which comprise the control laws for an axis are defined sequentially in
    the configuration file. For instance, for the X-15:

    <pre>
    \<FLIGHT_CONTROL NAME="X-15 SAS">

    \<COMPONENT NAME="Pitch Trim Sum" TYPE="SUMMER">
       INPUT        fcs/elevator-cmd-norm
       INPUT        fcs/pitch-trim-cmd-norm
       CLIPTO       -1 1
    \</COMPONENT>

    \<COMPONENT NAME="Pitch Command Scale" TYPE="AEROSURFACE_SCALE">
      INPUT        fcs/pitch-trim-sum
      MIN         -50
      MAX          50
    \</COMPONENT>

    \<COMPONENT NAME="Pitch Gain 1" TYPE="PURE_GAIN">
      INPUT        fcs/pitch-command-scale
      GAIN         -0.36
    \</COMPONENT>

    ... etc.
    </pre>

    In the above case we can see the first few components of the pitch channel
    defined. The input to the first component, as can be seen in the "Pitch trim
    sum" component, is really the sum of two parameters: elevator command (from
    the stick - a pilot input), and pitch trim. The type of this component is
    "Summer".
    The next component created is an aerosurface scale component - a type of
    gain (see the LoadFCS() method for insight on how the various types of
    components map into the actual component classes).  This continues until the
    final component for an axis when the
    OUTPUT keyword specifies where the output is supposed to go. See the
    individual components for more information on how they are mechanized.

    Another option for the flight controls portion of the config file is that in
    addition to using the "NAME" attribute in,

    <pre>
    \<FLIGHT_CONTROL NAME="X-15 SAS">
    </pre>

    one can also supply a filename:

    <pre>
    \<FLIGHT_CONTROL NAME="X-15 SAS" FILE="X15.xml">
    \</FLIGHT_CONTROL>
    </pre>

    In this case, the FCS would be read in from another file.

    @author Jon S. Berndt
    @version $Id: FGFCS.h,v 1.3 2005/06/13 16:59:17 ehofman Exp $
    @see FGFCSComponent
    @see FGXMLElement
    @see FGGain
    @see FGSummer
    @see FGSwitch
    @see FGGradient
    @see FGFilter
    @see FGDeadBand
*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DECLARATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

class FGFCS : public FGModel {

public:
  /** Constructor
      @param Executive a pointer to the parent executive object */
  FGFCS(FGFDMExec*);
  /// Destructor
  ~FGFCS();

  /** Runs the Flight Controls model; called by the Executive
      @return false if no error */
  bool Run(void);

  /// @name Pilot input command retrieval
  //@{
  /** Gets the aileron command.
      @return aileron command in range from -1.0 - 1.0 */
  inline double GetDaCmd(void) const { return DaCmd; }

  /** Gets the elevator command.
      @return elevator command in range from -1.0 - 1.0 */
  inline double GetDeCmd(void) const { return DeCmd; }

  /** Gets the rudder command.
      @return rudder command in range from -1.0 - 1.0 */
  inline double GetDrCmd(void) const { return DrCmd; }

  /** Gets the steering command.
      @return steering command in range from -1.0 - 1.0 */
  inline double GetDsCmd(void) const { return DsCmd; }

  /** Gets the flaps command.
      @return flaps command in range from 0 to 1.0 */
  inline double GetDfCmd(void) const { return DfCmd; }

  /** Gets the speedbrake command.
      @return speedbrake command in range from 0 to 1.0 */
  inline double GetDsbCmd(void) const { return DsbCmd; }

  /** Gets the spoiler command.
      @return spoiler command in range from 0 to 1.0 */
  inline double GetDspCmd(void) const { return DspCmd; }

  /** Gets the throttle command.
      @param engine engine ID number
      @return throttle command in range from 0 - 1.0 for the given engine */
  double GetThrottleCmd(int engine) const;

  /** Gets the mixture command.
      @param engine engine ID number
      @return mixture command in range from 0 - 1.0 for the given engine */
  inline double GetMixtureCmd(int engine) const { return MixtureCmd[engine]; }

  /** Gets the prop pitch command.
      @param engine engine ID number
      @return pitch command in range from 0.0 - 1.0 for the given engine */
  inline double GetPropAdvanceCmd(int engine) const { return PropAdvanceCmd[engine]; }

  /** Gets the pitch trim command.
      @return pitch trim command in range from -1.0 to 1.0 */
  inline double GetPitchTrimCmd(void) const { return PTrimCmd; }

  /** Gets the rudder trim command.
      @return rudder trim command in range from -1.0 - 1.0 */
  inline double GetYawTrimCmd(void) const { return YTrimCmd; }

  /** Gets the aileron trim command.
      @return aileron trim command in range from -1.0 - 1.0 */
  inline double GetRollTrimCmd(void) const { return RTrimCmd; }

  /** Get the gear extend/retract command. 0 commands gear up, 1 down.
      defaults to down.
      @return the current value of the gear extend/retract command*/
  inline double GetGearCmd(void) const { return GearCmd; }
  //@}

  /// @name Aerosurface position retrieval
  //@{
  /** Gets the left aileron position.
      @return aileron position in radians */
  inline double GetDaLPos( int form = ofRad )
                         const { return DaLPos[form]; }

  /// @name Aerosurface position retrieval
  //@{
  /** Gets the right aileron position.
      @return aileron position in radians */
  inline double GetDaRPos( int form = ofRad )
                         const { return DaRPos[form]; }

  /** Gets the elevator position.
      @return elevator position in radians */
  inline double GetDePos( int form = ofRad )
                         const { return DePos[form]; }

  /** Gets the rudder position.
      @return rudder position in radians */
  inline double GetDrPos( int form = ofRad )
                         const { return DrPos[form]; }

  /** Gets the speedbrake position.
      @return speedbrake position in radians */
  inline double GetDsbPos( int form = ofRad )
                         const { return DsbPos[form]; }

  /** Gets the spoiler position.
      @return spoiler position in radians */
  inline double GetDspPos( int form = ofRad )
                         const { return DspPos[form]; }

  /** Gets the flaps position.
      @return flaps position in radians */
  inline double GetDfPos( int form = ofRad )
                         const { return DfPos[form]; }

  /** Gets the throttle position.
      @param engine engine ID number
      @return throttle position for the given engine in range from 0 - 1.0 */
  double GetThrottlePos(int engine) const;

  /** Gets the mixture position.
      @param engine engine ID number
      @return mixture position for the given engine in range from 0 - 1.0 */
  inline double GetMixturePos(int engine) const { return MixturePos[engine]; }

  /** Gets the steering position.
      @return steering position in degrees */
  double GetSteerPosDeg(int gear) const { return SteerPosDeg[gear]; }

  /** Gets the gear position (0 up, 1 down), defaults to down
      @return gear position (0 up, 1 down) */
  inline double GetGearPos(void) const { return GearPos; }

  /** Gets the prop pitch position.
      @param engine engine ID number
      @return prop pitch position for the given engine in range from 0 - 1.0 */
  inline double GetPropAdvance(int engine) const { return PropAdvance[engine]; }
  //@}

  /** Retrieves the State object pointer.
      This is used by the FGFCS-owned components.
      @return pointer to the State object */
  inline FGState* GetState(void) { return State; }

  /** Retrieves all component names for inclusion in output stream
      @param delimeter either a tab or comma string depending on output type
      @return a string containing the descriptive names for all components */
  string GetComponentStrings(string delimeter);

  /** Retrieves all component outputs for inclusion in output stream
      @param delimeter either a tab or comma string depending on output type
      @return a string containing the numeric values for the current set of
      component outputs */
  string GetComponentValues(string delimeter);

  /// @name Pilot input command setting
  //@{
  /** Sets the aileron command
      @param cmd aileron command */
  inline void SetDaCmd( double cmd ) { DaCmd = cmd; }

  /** Sets the elevator command
      @param cmd elevator command in percent*/
  inline void SetDeCmd(double cmd ) { DeCmd = cmd; }

  /** Sets the rudder command
      @param cmd rudder command in percent*/
  inline void SetDrCmd(double cmd) { DrCmd = cmd; }

  /** Sets the steering command
      @param cmd steering command in percent*/
  inline void SetDsCmd(double cmd) { DsCmd = cmd; }

  /** Sets the flaps command
      @param cmd flaps command in percent*/
  inline void SetDfCmd(double cmd) { DfCmd = cmd; }

  /** Sets the speedbrake command
      @param cmd speedbrake command in percent*/
  inline void SetDsbCmd(double cmd) { DsbCmd = cmd; }

  /** Sets the spoilers command
      @param cmd spoilers command in percent*/
  inline void SetDspCmd(double cmd) { DspCmd = cmd; }

  /** Sets the pitch trim command
      @param cmd pitch trim command in percent*/
  inline void SetPitchTrimCmd(double cmd) { PTrimCmd = cmd; }

  /** Sets the rudder trim command
      @param cmd rudder trim command in percent*/
  inline void SetYawTrimCmd(double cmd) { YTrimCmd = cmd; }

  /** Sets the aileron trim command
      @param cmd aileron trim command in percent*/
  inline void SetRollTrimCmd(double cmd) { RTrimCmd = cmd; }

  /** Sets the throttle command for the specified engine
      @param engine engine ID number
      @param cmd throttle command in percent (0 - 100)*/
  void SetThrottleCmd(int engine, double cmd);

  /** Sets the mixture command for the specified engine
      @param engine engine ID number
      @param cmd mixture command in percent (0 - 100)*/
  void SetMixtureCmd(int engine, double cmd);

  /** Set the gear extend/retract command, defaults to down
      @param gear command 0 for up, 1 for down */
   void SetGearCmd(double gearcmd) { GearCmd = gearcmd; }

  /** Sets the propeller pitch command for the specified engine
      @param engine engine ID number
      @param cmd mixture command in percent (0.0 - 1.0)*/
  void SetPropAdvanceCmd(int engine, double cmd);
  //@}

  /// @name Aerosurface position setting
  //@{
  /** Sets the left aileron position
      @param cmd left aileron position in radians*/
  inline void SetDaLPos( int form , double pos )
                                      { DaLPos[form] = pos; }

  /** Sets the right aileron position
      @param cmd right aileron position in radians*/
  inline void SetDaRPos( int form , double pos )
                                      { DaRPos[form] = pos; }

  /** Sets the elevator position
      @param cmd elevator position in radians*/
  inline void SetDePos( int form , double pos )
                                      { DePos[form] = pos; }

  /** Sets the rudder position
      @param cmd rudder position in radians*/
  inline void SetDrPos( int form , double pos )
                                      { DrPos[form] = pos; }

   /** Sets the flaps position
      @param cmd flaps position in radians*/
  inline void SetDfPos( int form , double pos )
                                      { DfPos[form] = pos; }

  /** Sets the speedbrake position
      @param cmd speedbrake position in radians*/
  inline void SetDsbPos( int form , double pos )
                                      { DsbPos[form] = pos; }

  /** Sets the spoiler position
      @param cmd spoiler position in radians*/
  inline void SetDspPos( int form , double pos )
                                      { DspPos[form] = pos; }

  /** Sets the actual throttle setting for the specified engine
      @param engine engine ID number
      @param cmd throttle setting in percent (0 - 100)*/
  void SetThrottlePos(int engine, double cmd);

  /** Sets the actual mixture setting for the specified engine
      @param engine engine ID number
      @param cmd mixture setting in percent (0 - 100)*/
  void SetMixturePos(int engine, double cmd);

  /** Sets the steering position
      @param cmd steering position in degrees*/
  void SetSteerPosDeg(int gear, double pos) { SteerPosDeg[gear] = pos; }

  /** Set the gear extend/retract position, defaults to down
      @param gear position 0 up, 1 down       */
   void SetGearPos(double gearpos) { GearPos = gearpos; }


  /** Sets the actual prop pitch setting for the specified engine
      @param engine engine ID number
      @param cmd prop pitch setting in percent (0.0 - 1.0)*/
  void SetPropAdvance(int engine, double cmd);
  //@}

    /// @name Landing Gear brakes
  //@{
  /** Sets the left brake group
      @param cmd brake setting in percent (0.0 - 1.0) */
  void SetLBrake(double cmd) {LeftBrake = cmd;}

  /** Sets the right brake group
      @param cmd brake setting in percent (0.0 - 1.0) */
  void SetRBrake(double cmd) {RightBrake = cmd;}

  /** Sets the center brake group
      @param cmd brake setting in percent (0.0 - 1.0) */
  void SetCBrake(double cmd) {CenterBrake = cmd;}

  /** Gets the brake for a specified group.
      @param bg which brakegroup to retrieve the command for
      @return the brake setting for the supplied brake group argument */
  double GetBrake(FGLGear::BrakeGroup bg);
  //@}

  /** Loads the Flight Control System.
      The FGAircraft instance is actually responsible for reading the config file
      and calling the various Load() methods of the other systems, passing in
      the XML Element instance pointer. Load() is called from FGAircraft.
      @param el pointer to the Element instance
      @return true if succesful */
  bool Load(Element* el);

  void AddThrottle(void);
  void AddGear(void);

  FGPropertyManager* GetPropertyManager(void) { return PropertyManager; }

  void bind(void);
  void bindModel(void);
  void unbind(FGPropertyManager *node);

private:
  double DaCmd, DeCmd, DrCmd, DsCmd, DfCmd, DsbCmd, DspCmd;
  double DePos[NForms], DaLPos[NForms], DaRPos[NForms], DrPos[NForms];
  double DfPos[NForms], DsbPos[NForms], DspPos[NForms];
  double PTrimCmd, YTrimCmd, RTrimCmd;
  vector <double> ThrottleCmd;
  vector <double> ThrottlePos;
  vector <double> MixtureCmd;
  vector <double> MixturePos;
  vector <double> PropAdvanceCmd;
  vector <double> PropAdvance;
  vector <double> SteerPosDeg;
  double LeftBrake, RightBrake, CenterBrake; // Brake settings
  double GearCmd,GearPos;

  bool DoNormalize;
  void Normalize(void);

  vector <FGFCSComponent*> FCSComponents;
  vector <FGFCSComponent*> APComponents;
  vector <double*> interface_properties;
  int ToNormalize[NNorm];
  void Debug(int from);
};
}

#endif
