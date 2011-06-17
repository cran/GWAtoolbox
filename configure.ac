#
# Copyright © 2011 Daniel Taliun, Christian Fuchsberger and Cristian Pattaro. All rights reserved.
#
# This file is part of GWAtoolbox.
#
# GWAtoolbox is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# GWAtoolbox is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GWAtoolbox.  If not, see <http://www.gnu.org/licenses/>.
#

AC_INIT([GWAtoolbox], 1.1.0)

AC_PROG_CC

AC_PATH_PROG([GSL_CONFIG], [gsl-config])
if test "${GSL_CONFIG}" != ""; then
    GSL_CFLAGS=`${GSL_CONFIG} --cflags`
    GSL_LIBS=`${GSL_CONFIG} --libs`
else
    AC_MSG_ERROR([gsl-config was not found, check if GSL is installed.])
fi

AC_SUBST(GSL_CFLAGS)
AC_SUBST(GSL_LIBS)

AC_OUTPUT(src/Makevars)