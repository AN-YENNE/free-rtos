void vATaskFunction( void * pvParameters )
 { 
    /* 
     * Stack-allocated variables can be declared normally when inside a function. 
     * Each instance of a task created using this example function will have its 
     * own separate instance of lStackVariable allocated on the task's stack. 
     */ 
long lStackVariable = 0; 
    /* 
     * In contrast to stack allocated variables, variables declared with the `static` 
     * keyword are allocated to a specific location in memory by the linker. 
     * This means that all tasks calling vATaskFunction will share the same 
     * instance of lStaticVariable. 
     */ 
static long lStaticVariable = 0; 
for( ;; ) 
    { 
    /* The code to implement the task functionality will go here. */ 
    } 
    /* 
     * If the task implementation ever exits the above loop, then the task 
     * must be deleted before reaching the end of its implementing function. 
     * When NULL is passed as a parameter to the vTaskDelete() API function, 
     * this indicates that the task to be deleted is the calling (this) task. 
     */ 
    vTaskDelete( NULL ); 
} 