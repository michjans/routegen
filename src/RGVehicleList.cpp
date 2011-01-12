/*
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QDir>
#include <QDebug>

#include "RGSettings.h"
#include "RGVehicleList.h"

RGVehicleList::RGVehicleList()
{
  QDir vehicleDir = QDir::currentPath() + "/vehicles";
  QStringList filters;
  filters << "*.bmp" << "*.gif" << "*.png" << "*.jpg" << "*.tif";
  vehicleDir.setNameFilters(filters);
  QFileInfoList vehicles = vehicleDir.entryInfoList();
  RGVehicle *vehicle;
  vehicle= new RGVehicle();
  mMap.insert(0,vehicle);
  mCurrentVehicleId=0;
  int i=1;
  for (QFileInfoList::iterator it = vehicles.begin(); it != vehicles.end(); it++){
    vehicle= new RGVehicle(it->absoluteFilePath(),RGSettings::getVehicleSize(it->baseName()),
                           RGSettings::getVehicleMirrored(it->baseName()),RGSettings::getVehicleAngle(it->baseName()));
    if (vehicle->getRawSize()==0){
      delete vehicle;
      continue;
    }
    mMap.insert(i,vehicle);
    if(it->baseName()==RGSettings::getLastVehicleName())
      mCurrentVehicleId=i;
    i++;
  }
}

RGVehicleList::~RGVehicleList()
{
  qDebug()<<"RGVehicleList::~RGVehicleList()";
}

RGVehicle * RGVehicleList::getVehicle(int idx)
{
  if(mMap.count()>idx)
      return mMap.value(idx);
  return new RGVehicle();
}

int RGVehicleList::getCurrentVehicleId()
{
  return mCurrentVehicleId;
}

void RGVehicleList::setCurrentVehicleId(int idx)
{
  mCurrentVehicleId=idx;
  //save settings
  for (int i=1;i<mMap.count();i++){
    RGSettings::setVehicleAngle(mMap.value(i)->getName(),mMap.value(i)->getStartAngle());
    RGSettings::setVehicleSize(mMap.value(i)->getName(),mMap.value(i)->getSize());
    RGSettings::setVehicleMirrored(mMap.value(i)->getName(),mMap.value(i)->getMirror());
  }
  RGSettings::setLastVehicleName(mMap.value(idx)->getName());
}

int RGVehicleList::count()
{
  return mMap.count();
}
