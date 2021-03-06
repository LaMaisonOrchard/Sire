:

# Define the C++ compiler to user
GCC=g++ ; export GCC

# Define the install directory
if [ "${INSTALL}" = "" ]
then
    INSTALL=../install
fi

# Clean up any existing objects
rm *.o sire

# Make the build configuration file 
./build_data.ksh > build_v.h

# Make sire
make

# Install the files
cp ./sire* ${INSTALL}

if [ ! -d ${INSTALL}/sirerc ]
then
    mkdir ${INSTALL}/sirerc
fi

cp ./sirerc/*.src   ${INSTALL}/sirerc  # SET scripts
cp ./sirerc/*.help  ${INSTALL}/sirerc  # Help files
cp ./sirerc/*.tmpl  ${INSTALL}/sirerc  # Execute scripts
