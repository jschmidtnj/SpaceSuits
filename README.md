# SpaceSuits

A Hololens-based Space Suit.


### Standard Operation Procedure

1) Develop major changes on your own branch

2) Pull your master into your own branch when done

3) Test and make sure all features still work

4) Merge your branch into master

### C Sharp for Linux

`wget https://dist.nuget.org/win-x86-commandline/latest/nuget.exe`  
Then follow these steps:  
* https://www.mono-project.com/download/stable/#download-lin
* https://www.mono-project.com/docs/getting-started/mono-basics/
  
  
Finally try `mono nuget.exe install <Package-to-install>`

### git dev

1) `git checkout -b <branch name>`
2) `git add .`, `git commit -a "<message>"`
3) `git push --set-upstream origin <branch name>`
4) `git push origin`
5) `git pull origin master`
6) `git checkout master`
7) `git merge <branch name>`
8) `git add .`
9) `git commit -a "<message>"`
10) `git push origin master`

Notes: `git branch -av` lists the branches and the branch you are using  

### github pages update from master

`git subtree push --prefix groundcontrol-frontend-app/dist/ origin gh-pages`