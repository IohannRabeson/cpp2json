git config --global user.email "builds@travis-ci.com"
git config --global user.name "Travis CI"
git tag $VERSION_NUMBER -a -m "Generated tag from TravisCI for build $TRAVIS_BUILD_NUMBER."
git push origin $VERSION_NUMBER
