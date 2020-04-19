/*
    Copyright (C) 2009-2011  Michiel Jansen
    Copyright (C) 2010-2011  Fabien Valthier

  This file is part of Route Generator.

    Route Generator is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RGENCBMP2AVI_H
#define RGENCBMP2AVI_H

#include "RGEncVideo.h"

class RGEncBmp2avi : public RGEncVideo
{
  Q_OBJECT
public:

  RGEncBmp2avi(QWidget *parent = 0);
  void generateMovie(const QString &dirName) override;
  QString encoderName() override;
  QString encoderExecBaseName() override;
  QString frameFileType() const override;
  QString outputFileType() const override;

protected:
 bool initCodecs();

private:
  virtual void updateFromSettings();
  virtual void saveInSettings();
};

#endif // RGENCBMP2AVI_H
