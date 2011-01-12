/*
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RGVEHICLELIST_H
#define RGVEHICLELIST_H

#include <QMap>
#include "RGVehicle.h"

class RGVehicleList
{
public:
  RGVehicleList();
  ~RGVehicleList();
  RGVehicle* getVehicle(int);
  int getCurrentVehicleId();
  void setCurrentVehicleId(int);
  int count();

private:
  QMap<int,RGVehicle*>  mMap;
  int mCurrentVehicleId;
};

#endif // RGVEHICLELIST_H
