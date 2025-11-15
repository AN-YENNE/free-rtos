idf_component_register(
    SRCS
        #"01_task_core_affinity.c"
        #"02_task_deletion_example.c"
        "03_task_priority_example.c"
        #"04_task_suspend_resume.c"
        #"05_tasks_priority_change_at_runtime.c"

    INCLUDE_DIRS 
        "."
)