#include "environment/JvmEnvironment.h"
#include "EasyJni/EasyJni.h" // for ide

int main(int argc, char *argv[])
{
	testing::InitGoogleTest(&argc, argv);
	testing::AddGlobalTestEnvironment(jvmEnvironment);
	return RUN_ALL_TESTS();
}