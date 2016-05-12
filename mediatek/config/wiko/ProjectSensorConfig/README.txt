此文档主要介绍(陀螺仪+Gsensor+地磁)和(Gsensor+地磁)两种配置区别：

1、如果项目有带陀螺仪：
	 1、请将Gyro+Gsensor+Msensor下的ProjectConfig.mk拷贝到工程目录，重新合并ProjectConfig.mk文件
	 2、请将Gyro+Gsensor+Msensor下的codegen.dws文件拷贝到mediatek\custom\project\kernel\dct\dct下并替换原有的文件
	 

2、如果项目没有带陀螺仪：
	 1、请将Gsensor+Msensor下的ProjectConfig.mk拷贝到工程目录，重新合并ProjectConfig.mk文件
	 2、请将Gsensor+Msensor下的codegen.dws文件拷贝到mediatek\custom\project\kernel\dct\dct下并替换原有的文件
	 
注意：ProjectConfig.mk文件只是CUSTOM_KERNEL_ACCELEROMETER、CUSTOM_KERNEL_GYROSCOPE、MTK_AUTO_DETECT_ACCELEROMETER这三个宏不同，
			其他的不同请忽略，谢谢。