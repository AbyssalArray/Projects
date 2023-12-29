#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>

int main()
{
   int pid, ret_exec, status;
   pid=fork();

   switch(pid){

      case -1:
         /* code for case -1 */
         perror("Error");
         exit(1);
      case 0:
         /* code for case 0 */
         ret_exec = execl("/bin/ps", "ps", "-A", NULL);
         if (ret_exec == -1) {
            perror("Error executing exec");
            exit(1);
         }
         break;
      default:
         /* code for case default */

         pid = wait(&status);

         if (WIFEXITED(status)) {
            printf("%d\n", getpid());
            printf("%d\n", pid);
            printf("%d\n", WEXITSTATUS(status));
         }
   }
   return 0;
}
